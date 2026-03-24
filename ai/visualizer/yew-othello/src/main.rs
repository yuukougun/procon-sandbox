use yew::prelude::*;

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
    let board = use_state(|| {
        let mut b = vec![Stone::Empty; SIZE*SIZE];
        b[27] = Stone::White; b[28] = Stone::Black;
        b[35] = Stone::Black; b[36] = Stone::White;
        b
    });
    let turn = use_state(|| Stone::Black);
    let game_over = use_state(|| false);

    use std::rc::Rc;
    let legal = Rc::new(get_legal_moves(&board, *turn));
    let (b, w) = count_stones(&board);

    let onclick = {
        let board = board.clone();
        let turn = turn.clone();
        let game_over = game_over.clone();
        let legal = legal.clone();
        Callback::from(move |pos: usize| {
            if *game_over { return; }
            if !legal.contains(&pos) { return; }
            let new_board = do_move((*board).clone(), pos, *turn);
            let next_turn = opponent(*turn);
            let next_legal = get_legal_moves(&new_board, next_turn);
            if next_legal.is_empty() {
                let again_legal = get_legal_moves(&new_board, *turn);
                if again_legal.is_empty() {
                    game_over.set(true);
                } else {
                    board.set(new_board);
                    // パス
                    return;
                }
            }
            board.set(new_board);
            turn.set(next_turn);
        })
    };

    html! {
        <div style="text-align:center;">
            <h1>{ "Yew Othello" }</h1>
            <div style="display:grid;grid-template-columns:repeat(8,40px);grid-template-rows:repeat(8,40px);width:fit-content;margin:0 auto;border:4px solid #333;background:#228b22;">
                { for (0..SIZE*SIZE).map(|i| {
                    let s = board[i];
                    let cell_style = if legal.contains(&i) {
                        "width:40px;height:40px;border:1px solid #333;box-sizing:border-box;cursor:pointer;background:#228b22;display:flex;align-items:center;justify-content:center;box-shadow:inset 0 0 0 3px #ffe45c;"
                    } else {
                        "width:40px;height:40px;border:1px solid #333;box-sizing:border-box;cursor:pointer;background:#228b22;display:flex;align-items:center;justify-content:center;"
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

                    html! {
                        <div style={cell_style}
                            onclick={let onclick = onclick.clone(); Callback::from(move |_| onclick.emit(i))}>
                            { stone }
                        </div>
                    }
                }) }
            </div>
            <div style="margin:10px;font-size:1.2em;">
                { format!("手番: {}　黒:{} 白:{}", if *turn == Stone::Black { "黒" } else { "白" }, b, w) }
            </div>
            if *game_over {
                <div style="color:red;font-weight:bold;">{ "ゲーム終了" }</div>
            }
            <button onclick={
                let board = board.clone();
                let turn = turn.clone();
                let game_over = game_over.clone();
                Callback::from(move |_| {
                    let mut b = vec![Stone::Empty; SIZE*SIZE];
                    b[27] = Stone::White; b[28] = Stone::Black;
                    b[35] = Stone::Black; b[36] = Stone::White;
                    board.set(b);
                    turn.set(Stone::Black);
                    game_over.set(false);
                })
            }>{ "リセット" }</button>
        </div>
    }
}

fn main() {
    yew::Renderer::<App>::new().render();
}
