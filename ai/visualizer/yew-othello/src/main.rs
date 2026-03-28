use console_error_panic_hook;
use gloo_net::http::Request;
use wasm_bindgen_futures::spawn_local;
use yew::prelude::*;

const SIZE: usize = 8;

#[derive(Clone, Copy, PartialEq, Eq)]
enum Stone {
    Empty,
    Black,
    White,
}

const HUMAN_COLOR: Stone = Stone::Black;
const AI_COLOR: Stone = Stone::White;

fn initial_board() -> Vec<Stone> {
    let mut b = vec![Stone::Empty; SIZE * SIZE];
    b[27] = Stone::White;
    b[28] = Stone::Black;
    b[35] = Stone::Black;
    b[36] = Stone::White;
    b
}

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
    for pos in 0..SIZE * SIZE {
        if board[pos] != Stone::Empty {
            continue;
        }
        if can_flip(board, pos, turn) {
            moves.push(pos);
        }
    }
    moves
}

fn can_flip(board: &[Stone], pos: usize, turn: Stone) -> bool {
    let x = (pos % SIZE) as i32;
    let y = (pos / SIZE) as i32;
    let opp = opponent(turn);
    let dx = [1, -1, 0, 0, 1, -1, 1, -1];
    let dy = [0, 0, 1, -1, 1, -1, -1, 1];
    for d in 0..8 {
        let mut nx = x + dx[d];
        let mut ny = y + dy[d];
        let mut found = false;
        let mut cnt = 0;
        while in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == opp {
                found = true;
                nx += dx[d];
                ny += dy[d];
                cnt += 1;
            } else {
                break;
            }
        }
        if found && cnt > 0 && in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == turn {
                return true;
            }
        }
    }
    false
}

fn do_move(mut board: Vec<Stone>, pos: usize, turn: Stone) -> Vec<Stone> {
    let x = (pos % SIZE) as i32;
    let y = (pos / SIZE) as i32;
    let opp = opponent(turn);
    let dx = [1, -1, 0, 0, 1, -1, 1, -1];
    let dy = [0, 0, 1, -1, 1, -1, -1, 1];
    let mut flipped = vec![];
    for d in 0..8 {
        let mut nx = x + dx[d];
        let mut ny = y + dy[d];
        let mut line = vec![];
        while in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == opp {
                line.push(np);
                nx += dx[d];
                ny += dy[d];
            } else {
                break;
            }
        }
        if !line.is_empty() && in_board(nx, ny) {
            let np = (ny as usize) * SIZE + (nx as usize);
            if board[np] == turn {
                flipped.extend(line);
            }
        }
    }
    if flipped.is_empty() {
        return board;
    }
    board[pos] = turn;
    for &f in &flipped {
        board[f] = turn;
    }
    board
}

fn count_stones(board: &[Stone]) -> (usize, usize) {
    let mut b = 0;
    let mut w = 0;
    for &s in board {
        if s == Stone::Black {
            b += 1;
        } else if s == Stone::White {
            w += 1;
        }
    }
    (b, w)
}

fn board_to_bitboards(board: &[Stone]) -> (u64, u64) {
    let mut black = 0u64;
    let mut white = 0u64;
    for (idx, stone) in board.iter().enumerate() {
        match stone {
            Stone::Black => black |= 1u64 << idx,
            Stone::White => white |= 1u64 << idx,
            Stone::Empty => {}
        }
    }
    (black, white)
}

fn resolve_next_turn(board: &[Stone], played_turn: Stone) -> (Stone, bool, bool) {
    let next_turn = opponent(played_turn);
    let next_legal = get_legal_moves(board, next_turn);
    if !next_legal.is_empty() {
        return (next_turn, false, false);
    }

    let again_legal = get_legal_moves(board, played_turn);
    if again_legal.is_empty() {
        return (next_turn, true, false);
    }

    (played_turn, false, true)
}

async fn request_ai_move(
    api_base: String,
    model_path: String,
    board: Vec<Stone>,
    black_to_move: bool,
) -> Result<i32, String> {
    let (black, white) = board_to_bitboards(&board);
    let encoded_model = urlencoding::encode(&model_path);
    let base = api_base.trim_end_matches('/');
    let url = format!(
        "{}/ai-move?black={}&white={}&black_to_move={}&model_path={}",
        base,
        black,
        white,
        if black_to_move { 1 } else { 0 },
        encoded_model,
    );

    let resp = Request::get(&url)
        .send()
        .await
        .map_err(|e| format!("AIサーバーへ接続できません: {}", e))?;

    if !resp.ok() {
        let status = resp.status();
        let body = resp
            .text()
            .await
            .unwrap_or_else(|_| "(response parse error)".to_string());
        return Err(format!("AIサーバーエラー: status={} body={}", status, body));
    }

    let text = resp
        .text()
        .await
        .map_err(|e| format!("AI応答の読み取り失敗: {}", e))?;

    text.trim()
        .parse::<i32>()
        .map_err(|e| format!("AI応答のパース失敗: {} (raw={})", e, text.trim()))
}

fn turn_label(turn: Stone) -> &'static str {
    match turn {
        Stone::Black => "黒",
        Stone::White => "白",
        Stone::Empty => "-",
    }
}

#[function_component(App)]
fn app() -> Html {
    let file_name = use_state(|| "/data/game.csv".to_string());
    let api_base = use_state(|| "http://127.0.0.1:8000".to_string());
    let model_path = use_state(|| "ai/data/models".to_string());
    let ai_enabled = use_state(|| true);
    let ai_thinking = use_state(|| false);
    let status_text = use_state(|| "あなたは黒です。黒をクリックして開始してください。".to_string());

    let boards = use_state(|| vec![initial_board()]);
    let history_idx = use_state(|| 0usize);
    let turn = use_state(|| Stone::Black);
    let game_over = use_state(|| false);

    let boards_setter = boards.clone();
    let history_idx_setter = history_idx.clone();
    let turn_setter = turn.clone();
    let game_over_setter = game_over.clone();
    let status_setter = status_text.clone();
    let ai_thinking_setter = ai_thinking.clone();
    let file_name_state = file_name.clone();
    let on_load_csv = {
        Callback::from(move |_| {
            let boards_setter = boards_setter.clone();
            let history_idx_setter = history_idx_setter.clone();
            let turn_setter = turn_setter.clone();
            let game_over_setter = game_over_setter.clone();
            let status_setter = status_setter.clone();
            let ai_thinking_setter = ai_thinking_setter.clone();
            let file_name = file_name_state.to_string();
            spawn_local(async move {
                let resp = Request::get(&file_name).send().await;
                if let Ok(resp) = resp {
                    if let Ok(text) = resp.text().await {
                        let mut new_boards = Vec::new();
                        for (i, line) in text.lines().enumerate() {
                            if i == 0 {
                                continue;
                            }
                            let cols: Vec<&str> = line.split(',').collect();
                            if cols.len() < 5 {
                                continue;
                            }
                            let black = match cols[0].parse::<u64>() {
                                Ok(v) => v,
                                Err(_) => continue,
                            };
                            let white = match cols[1].parse::<u64>() {
                                Ok(v) => v,
                                Err(_) => continue,
                            };
                            let mut b = vec![Stone::Empty; SIZE * SIZE];
                            for idx in 0..64 {
                                if (black >> idx) & 1 == 1 {
                                    b[idx] = Stone::Black;
                                } else if (white >> idx) & 1 == 1 {
                                    b[idx] = Stone::White;
                                }
                            }
                            new_boards.push(b);
                        }
                        if !new_boards.is_empty() {
                            boards_setter.set(new_boards);
                            history_idx_setter.set(0);
                            turn_setter.set(Stone::Black);
                            game_over_setter.set(false);
                            ai_thinking_setter.set(false);
                            status_setter.set("CSVリプレイを読み込みました。".to_string());
                        }
                    }
                }
            });
        })
    };

    let board = &boards[*history_idx];
    let legal = get_legal_moves(board, *turn);
    let (b, w) = count_stones(board);

    let onclick = {
        let boards = boards.clone();
        let history_idx = history_idx.clone();
        let turn = turn.clone();
        let game_over = game_over.clone();
        let ai_enabled = ai_enabled.clone();
        let ai_thinking = ai_thinking.clone();
        let status_text = status_text.clone();
        let api_base = api_base.clone();
        let model_path = model_path.clone();
        let legal = legal.clone();
        Callback::from(move |pos: usize| {
            if *game_over || *ai_thinking || *turn != HUMAN_COLOR {
                return;
            }
            if !legal.contains(&pos) {
                return;
            }

            let current_board = boards[*history_idx].clone();
            let board_after_human = do_move(current_board, pos, *turn);
            let (turn_after_human, game_over_after_human, opponent_passed_after_human) =
                resolve_next_turn(&board_after_human, *turn);

            let mut boards_after_human = boards[..=*history_idx].to_vec();
            boards_after_human.push(board_after_human.clone());
            let human_history_idx = boards_after_human.len() - 1;

            boards.set(boards_after_human.clone());
            history_idx.set(human_history_idx);
            turn.set(turn_after_human);
            game_over.set(game_over_after_human);

            if game_over_after_human {
                status_text.set("ゲーム終了".to_string());
                return;
            }

            if opponent_passed_after_human {
                status_text.set("白がパスしました。続けて黒の手番です。".to_string());
                return;
            }

            if !*ai_enabled || turn_after_human != AI_COLOR {
                status_text.set("あなたの手番です。".to_string());
                return;
            }

            ai_thinking.set(true);
            status_text.set("AI思考中...".to_string());

            let boards_setter = boards.clone();
            let history_idx_setter = history_idx.clone();
            let turn_setter = turn.clone();
            let game_over_setter = game_over.clone();
            let ai_thinking_setter = ai_thinking.clone();
            let status_setter = status_text.clone();

            let mut work_board = board_after_human.clone();
            let mut work_boards = boards_after_human.clone();
            let api_base_value = (*api_base).clone();
            let model_path_value = (*model_path).clone();

            spawn_local(async move {
                let (final_turn, final_game_over) = loop {
                    let ai_legal = get_legal_moves(&work_board, AI_COLOR);
                    if ai_legal.is_empty() {
                        let human_legal = get_legal_moves(&work_board, HUMAN_COLOR);
                        break (HUMAN_COLOR, human_legal.is_empty());
                    }

                    let ai_result = request_ai_move(
                        api_base_value.clone(),
                        model_path_value.clone(),
                        work_board.clone(),
                        false,
                    )
                    .await;

                    let ai_pos = match ai_result {
                        Ok(v) => v,
                        Err(e) => {
                            ai_thinking_setter.set(false);
                            status_setter.set(format!("AI呼び出し失敗: {}", e));
                            return;
                        }
                    };

                    if ai_pos < 0 {
                        ai_thinking_setter.set(false);
                        status_setter.set("AIが不正なパスを返しました。".to_string());
                        return;
                    }

                    let ai_pos_usize = ai_pos as usize;
                    if !ai_legal.contains(&ai_pos_usize) {
                        ai_thinking_setter.set(false);
                        status_setter.set(format!("AIの着手が不正です: {}", ai_pos));
                        return;
                    }

                    work_board = do_move(work_board, ai_pos_usize, AI_COLOR);
                    work_boards.push(work_board.clone());

                    let (turn_after_ai, game_over_after_ai, human_passed_after_ai) =
                        resolve_next_turn(&work_board, AI_COLOR);
                    if game_over_after_ai || !human_passed_after_ai {
                        break (turn_after_ai, game_over_after_ai);
                    }
                };

                let ai_history_idx = work_boards.len() - 1;
                boards_setter.set(work_boards);
                history_idx_setter.set(ai_history_idx);
                turn_setter.set(final_turn);
                game_over_setter.set(final_game_over);
                ai_thinking_setter.set(false);

                if final_game_over {
                    status_setter.set("ゲーム終了".to_string());
                } else {
                    status_setter.set("あなたの手番です。".to_string());
                }
            });
        })
    };

    let on_back = {
        let history_idx = history_idx.clone();
        let ai_thinking = ai_thinking.clone();
        Callback::from(move |_| {
            if *history_idx > 0 && !*ai_thinking {
                history_idx.set(*history_idx - 1);
            }
        })
    };

    let on_forward = {
        let boards = boards.clone();
        let history_idx = history_idx.clone();
        let ai_thinking = ai_thinking.clone();
        Callback::from(move |_| {
            if *history_idx + 1 < boards.len() && !*ai_thinking {
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

            <div style="margin-bottom:10px;display:flex;gap:8px;justify-content:center;flex-wrap:wrap;">
                <label>{ "AI API" }</label>
                <input
                    type="text"
                    value={(*api_base).clone()}
                    oninput={
                        let api_base = api_base.clone();
                        Callback::from(move |e: InputEvent| {
                            let input = e.target_dyn_into::<web_sys::HtmlInputElement>().unwrap();
                            api_base.set(input.value());
                        })
                    }
                    style="width:260px;"
                />

                <label>{ "Model" }</label>
                <input
                    type="text"
                    value={(*model_path).clone()}
                    oninput={
                        let model_path = model_path.clone();
                        Callback::from(move |e: InputEvent| {
                            let input = e.target_dyn_into::<web_sys::HtmlInputElement>().unwrap();
                            model_path.set(input.value());
                        })
                    }
                    style="width:320px;"
                />

                <label>
                    <input
                        type="checkbox"
                        checked={*ai_enabled}
                        onchange={
                            let ai_enabled = ai_enabled.clone();
                            Callback::from(move |e: Event| {
                                let input = e.target_dyn_into::<web_sys::HtmlInputElement>().unwrap();
                                ai_enabled.set(input.checked());
                            })
                        }
                    />
                    { " 白をAIにする" }
                </label>
            </div>

            <h1>{ "Yew Othello" }</h1>

            <div style="display:grid;grid-template-columns:repeat(8,40px);grid-template-rows:repeat(8,40px);width:fit-content;margin:0 auto;border:4px solid #333;background:#228b22;">
                { for (0..SIZE * SIZE).map(|i| {
                    let s = board[i];
                    let cell_style = {
                        let legal = legal.clone();
                        let history_idx = history_idx.clone();
                        let boards = boards.clone();
                        let game_over = game_over.clone();
                        let turn = turn.clone();
                        let ai_thinking = ai_thinking.clone();
                        if legal.contains(&i)
                            && *history_idx + 1 == boards.len()
                            && !*game_over
                            && !*ai_thinking
                            && *turn == HUMAN_COLOR
                        {
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
                        let ai_thinking = ai_thinking.clone();
                        let turn = turn.clone();
                        Callback::from(move |_| {
                            if *history_idx + 1 == boards.len()
                                && !*game_over
                                && !*ai_thinking
                                && *turn == HUMAN_COLOR
                            {
                                onclick.emit(i);
                            }
                        })
                    };

                    html! {
                        <div style={cell_style} onclick={onclick}>
                            { stone }
                        </div>
                    }
                }) }
            </div>

            <div style="margin:10px;font-size:1.2em;">
                { format!("手番: {} 黒:{} 白:{}", turn_label(*turn), b, w) }
            </div>

            <div style="margin:6px 0;color:#333;">
                {
                    if *ai_thinking {
                        "AI思考中...".to_string()
                    } else {
                        (*status_text).clone()
                    }
                }
            </div>

            if *game_over {
                <div style="color:red;font-weight:bold;">{ "ゲーム終了" }</div>
            }

            <div style="margin:10px;">
                <button onclick={on_back.clone()} disabled={*history_idx == 0 || *ai_thinking}>{ "巻き戻し" }</button>
                <span style="margin:0 10px;">{ format!("{}/{}", *history_idx + 1, boards.len()) }</span>
                <button onclick={on_forward.clone()} disabled={*history_idx + 1 == boards.len() || *ai_thinking}>{ "進める" }</button>
            </div>

            <button onclick={
                let boards = boards.clone();
                let history_idx = history_idx.clone();
                let turn = turn.clone();
                let game_over = game_over.clone();
                let ai_thinking = ai_thinking.clone();
                let status_text = status_text.clone();
                Callback::from(move |_| {
                    boards.set(vec![initial_board()]);
                    history_idx.set(0);
                    turn.set(Stone::Black);
                    game_over.set(false);
                    ai_thinking.set(false);
                    status_text.set("あなたは黒です。黒をクリックして開始してください。".to_string());
                })
            } disabled={*ai_thinking}>{ "リセット" }</button>
        </div>
    }
}

fn main() {
    console_error_panic_hook::set_once();
    yew::Renderer::<App>::new().render();
}
