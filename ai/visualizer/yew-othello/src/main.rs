use console_error_panic_hook;
use yew::prelude::*;
use gloo_net::http::Request;

const SIZE: usize = 8;
#[derive(Clone, Copy, PartialEq, Eq)]
enum Stone { Empty, Black, White }

fn opponent(turn: Stone) -> Stone {
    match turn {
        Stone::Black => Stone::White,
        Stone::White => Stone::Black,
        _ => Stone::Empty,
    }
}

fn in_board(x: i32, y: i32) -> bool {
    x >= 0 && x < SIZE as i32 && y >= 0 && y < SIZE as i32
}

fn get_legal_moves(board: &[Stone], turn: Stone) -> Vec<usize> {
    let mut moves = vec![];
    for pos in 0..SIZE*SIZE {
        if board[pos] != Stone::Empty { continue; }
        if can_flip(board, pos, turn) { moves.push(pos); }
    }
    moves
}

fn can_flip(board: &[Stone], pos: usize, turn: Stone) -> bool {
    let x = (pos % SIZE) as i32;
    let y = (pos / SIZE) as i32;
    let opp = opponent(turn);
    let dx = [1,-1,0,0,1,-1,1,-1];
    let dy = [0,0,1,-1,1,-1,-1,1];
    for d in 0..8 {
        let mut nx = x + dx[d];
        let mut ny = y + dy[d];
        let mut found = false;
        let mut cnt = 0;
        while in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == opp {
                found = true;
                nx += dx[d]; ny += dy[d]; cnt += 1;
            } else { break; }
        }
        if found && cnt > 0 && in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == turn { return true; }
        }
    }
    false
}

fn do_move(mut board: Vec<Stone>, pos: usize, turn: Stone) -> Vec<Stone> {
    let x = (pos % SIZE) as i32;
    let y = (pos / SIZE) as i32;
    let opp = opponent(turn);
    let dx = [1,-1,0,0,1,-1,1,-1];
    let dy = [0,0,1,-1,1,-1,-1,1];
    let mut flipped = vec![];
    for d in 0..8 {
        let mut nx = x + dx[d];
        let mut ny = y + dy[d];
        let mut line = vec![];
        while in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == opp {
                line.push(np);
                nx += dx[d]; ny += dy[d];
            } else { break; }
        }
        if !line.is_empty() && in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == turn { flipped.extend(line); }
        }
    }
    if flipped.is_empty() { return board; }
    board[pos] = turn;
    for &f in &flipped { board[f] = turn; }
    board
}

fn count_stones(board: &[Stone]) -> (usize, usize) {
    let mut b = 0; let mut w = 0;
    for &s in board {
        if s == Stone::Black { b += 1; }
        else if s == Stone::White { w += 1; }
    }
    (b, w)
}

#[function_component(App)]
fn app() -> Html {
    // ファイル名入力用state
    let file_name = use_state(|| "/data/game.csv".to_string());
    // input_refは未使用なので削除
    // 対局履歴（boards）と現在インデックス（history_idx）をuse_stateで管理
    let boards = use_state(|| {
        let mut b = vec![Stone::Empty; SIZE*SIZE];
        b[27] = Stone::White; b[28] = Stone::Black;
        b[35] = Stone::Black; b[36] = Stone::White;
        vec![b]
    });
    let history_idx = use_state(|| 0usize);
    let turn = use_state(|| Stone::Black);
    let game_over = use_state(|| false);

    // CSV読込・履歴反映
    let boards_setter = boards.clone();
    let history_idx_setter = history_idx.clone();
    let turn_setter = turn.clone();
    let game_over_setter = game_over.clone();
    let file_name_state = file_name.clone();
    let on_load_csv = {
        Callback::from(move |_| {
            web_sys::console::log_1(&"読込ボタン押下".into());
            let boards_setter = boards_setter.clone();
            let history_idx_setter = history_idx_setter.clone();
            let turn_setter = turn_setter.clone();
            let game_over_setter = game_over_setter.clone();
            let file_name = file_name_state.to_string();
            wasm_bindgen_futures::spawn_local(async move {
                web_sys::console::log_1(&format!("[CSV] 読込開始: {}", file_name).into());
                let resp = Request::get(&file_name).send().await;
                if let Ok(resp) = resp {
                    web_sys::console::log_1(&"[CSV] fetch成功".into());
                    if let Ok(text) = resp.text().await {
                        web_sys::console::log_1(&"[CSV] テキスト取得成功".into());
                        let mut new_boards = Vec::new();
                        for (i, line) in text.lines().enumerate() {
                            if i == 0 { continue; } // skip header
                            let cols: Vec<&str> = line.split(',').collect();
                            if cols.len() < 5 {
                                web_sys::console::error_1(&format!("[CSV] 列数が足りません: {}行目: {}", i+1, line).into());
                                continue;
                            }
                            let black = match cols[0].parse::<u64>() {
                                Ok(v) => v,
                                Err(e) => {
                                    web_sys::console::error_1(&format!("[CSV] black列のパース失敗: {}行目: {} ({})", i+1, cols[0], e).into());
                                    continue;
                                }
                            };
                            let white = match cols[1].parse::<u64>() {
                                Ok(v) => v,
                                Err(e) => {
                                    web_sys::console::error_1(&format!("[CSV] white列のパース失敗: {}行目: {} ({})", i+1, cols[1], e).into());
                                    continue;
                                }
                            };
                            let mut b = vec![Stone::Empty; SIZE*SIZE];
                            for idx in 0..64 {
                                if (black >> idx) & 1 == 1 {
                                    b[idx] = Stone::Black;
                                } else if (white >> idx) & 1 == 1 {
                                    b[idx] = Stone::White;
                                }
                            }
                            new_boards.push(b);
                        }
                        if new_boards.is_empty() {
                            web_sys::console::error_1(&"[CSV] 有効な棋譜が1件もありません".into());
                        } else {
                            let n = new_boards.len();
                            boards_setter.set(new_boards);
                            history_idx_setter.set(0);
                            turn_setter.set(Stone::Black); // CSVの手番情報を使う場合はここを拡張
                            game_over_setter.set(false);
                            web_sys::console::log_1(&format!("[CSV] 棋譜{}件読込", n).into());
                        }
                    } else {
                        web_sys::console::error_1(&"[CSV] レスポンスのテキスト取得に失敗".into());
                    }
                } else {
                    web_sys::console::error_1(&"[CSV] ファイルの取得に失敗".into());
                }
            });
        })
    };

    use std::rc::Rc;
    let board = &boards[*history_idx];
    let legal = Rc::new(get_legal_moves(board, *turn));
    let (b, w) = count_stones(board);

    let onclick = {
        let boards = boards.clone();
        let history_idx = history_idx.clone();
        let turn = turn.clone();
        let game_over = game_over.clone();
        let legal = legal.clone();
        Callback::from(move |pos: usize| {
            if *game_over { return; }
            if !legal.contains(&pos) { return; }
            let new_board = do_move(boards[*history_idx].clone(), pos, *turn);
            let next_turn = opponent(*turn);
            let next_legal = get_legal_moves(&new_board, next_turn);
            let mut new_boards = boards[..=*history_idx].to_vec();
            new_boards.push(new_board.clone());
            if next_legal.is_empty() {
                let again_legal = get_legal_moves(&new_board, *turn);
                if again_legal.is_empty() {
                    game_over.set(true);
                } else {
                    boards.set(new_boards);
                    history_idx.set(*history_idx + 1);
                    // パス
                    return;
                }
            }
            boards.set(new_boards);
            history_idx.set(*history_idx + 1);
            turn.set(next_turn);
        })
    };

    let on_back = {
        let history_idx = history_idx.clone();
        Callback::from(move |_| {
            if *history_idx > 0 {
                history_idx.set(*history_idx - 1);
            }
        })
    };
    let on_forward = {
        let boards = boards.clone();
        let history_idx = history_idx.clone();
        Callback::from(move |_| {
            if *history_idx + 1 < boards.len() {
                history_idx.set(*history_idx + 1);
            }
        })
    };

    html! {
        <div style="text-align:center;">
            <div style="margin-bottom:10px;">
                <input
                    type="text"
                    value={(*file_name).clone()}
                    oninput={
                        let file_name = file_name.clone();
                        Callback::from(move |e: InputEvent| {
                            let input = e.target_dyn_into::<web_sys::HtmlInputElement>().unwrap();
                            file_name.set(input.value());
                        })
                    }
                    style="width:300px;"
                />
                <button onclick={on_load_csv.clone()}>{ "読込" }</button>
                <span style="margin-left:10px;">{ "CSVファイルを指定してリプレイ" }</span>
            </div>
            <h1>{ "Yew Othello" }</h1>
            <div style="display:grid;grid-template-columns:repeat(8,40px);grid-template-rows:repeat(8,40px);width:fit-content;margin:0 auto;border:4px solid #333;background:#228b22;">
                { for (0..SIZE*SIZE).map(|i| {
                    let s = board[i];
                    let cell_style = {
                        let legal = legal.clone();
                        let history_idx = history_idx.clone();
                        let boards = boards.clone();
                        let game_over = game_over.clone();
                        if legal.contains(&i) && *history_idx + 1 == boards.len() && !*game_over {
                            "width:40px;height:40px;border:1px solid #333;box-sizing:border-box;cursor:pointer;background:#228b22;display:flex;align-items:center;justify-content:center;box-shadow:inset 0 0 0 3px #ffe45c;"
                        } else {
                            "width:40px;height:40px;border:1px solid #333;box-sizing:border-box;cursor:pointer;background:#228b22;display:flex;align-items:center;justify-content:center;"
                        }
                    };

                    let stone = match s {
                        Stone::Black => html! {
                            <div style="width:30px;height:30px;border-radius:50%;background:#111;"></div>
                        },
                        Stone::White => html! {
                            <div style="width:30px;height:30px;border-radius:50%;background:#fff;border:2px solid #aaa;box-sizing:border-box;"></div>
                        },
                        Stone::Empty => html! {},
                    };

                    let onclick = {
                        let onclick = onclick.clone();
                        let history_idx = history_idx.clone();
                        let boards = boards.clone();
                        let game_over = game_over.clone();
                        Callback::from(move |_| {
                            if *history_idx + 1 == boards.len() && !*game_over {
                                onclick.emit(i)
                            }
                        })
                    };

                    html! {
                        <div style={cell_style}
                            onclick={onclick}>
                            { stone }
                        </div>
                    }
                }) }
            </div>
            <div style="margin:10px;font-size:1.2em;">
                { format!("手番: {} 黒:{} 白:{}", if *turn == Stone::Black { "黒" } else { "白" }, b, w) }
            </div>
            if *game_over {
                <div style="color:red;font-weight:bold;">{ "ゲーム終了" }</div>
            }
            <div style="margin:10px;">
                <button onclick={on_back.clone()} disabled={*history_idx == 0}>{ "巻き戻し" }</button>
                <span style="margin:0 10px;">{ format!("{}/{}", *history_idx + 1, boards.len()) }</span>
                <button onclick={on_forward.clone()} disabled={*history_idx + 1 == boards.len()}>{ "進める" }</button>
            </div>
            <button onclick={
                let boards = boards.clone();
                let history_idx = history_idx.clone();
                let turn = turn.clone();
                let game_over = game_over.clone();
                Callback::from(move |_| {
                    let mut b = vec![Stone::Empty; SIZE*SIZE];
                    b[27] = Stone::White; b[28] = Stone::Black;
                    b[35] = Stone::Black; b[36] = Stone::White;
                    boards.set(vec![b]);
                    history_idx.set(0);
                    turn.set(Stone::Black);
                    game_over.set(false);
                })
            }>{ "リセット" }</button>
        </div>
    }
}

fn main() {
    // panic時にブラウザconsoleへエラー出力
    console_error_panic_hook::set_once();
    yew::Renderer::<App>::new().render();
}
