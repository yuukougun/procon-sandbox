use eframe::egui;

fn main() -> eframe::Result {
    let mut native_options = eframe::NativeOptions::default();
    native_options.viewport.inner_size = Some(egui::Vec2::new(200.0, 240.0));
    native_options.viewport.resizable = Some(false);
    eframe::run_native(
        "Calculator",
        native_options,
        Box::new(|_| Ok(Box::new(MyApp::default()))),
    )
}

#[derive(Default)]
struct MyApp {
    // 画面表示値
    display: i64,
    // 入力値
    input: i64,
    //　今までの値の合計
    stack: i64,
    // 次にされる演算(待機列)
    operation: Option<Operation>,
}

/// 四則演算
enum Operation {
    /// 加法
    Add,
    /// 減法
    Sub,
    /// 乗法
    Mul,
    /// 除法
    Div,
}

impl MyApp {
    /// 次の演算を受け取り待機列`operation`に加え、計算します。
    /// 引数`op`に`None`を受け取った場合、表示値がそのまま`stack`に流用されます。
    fn calc(&mut self, op: Option<Operation>) {
        match if let Some(op) = op {
            self.operation.replace(op)
        } else {
            self.operation.take()
        } {
            Some(op) => match op {
                Operation::Add => self.stack += self.input,
                Operation::Sub => self.stack -= self.input,
                Operation::Mul => self.stack *= self.input,
                Operation::Div => self.stack /= self.input,
            },
            None => self.stack = self.display,
        }
        self.display = self.stack;
        self.input = 0;
    }
}

impl eframe::App for MyApp {
    fn update(&mut self, ctx: &egui::Context, _: &mut eframe::Frame) {
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.label(egui::RichText::new(format!("{}", self.display)).size(40.0));
            egui::Grid::new("keyboard").show(ui, |ui| {
                /// イベントが設定された数字ボタンを配置するマクロ。
                macro_rules! add_num_button {
                    ($n: expr) => {
                        if ui
                            .add_sized([40.0, 40.0], egui::Button::new(format!("{}", $n)))
                            .clicked()
                        {
                            self.input = self.input * 10 + $n;
                            self.display = self.input;
                        }
                    };
                }

                add_num_button!(7);
                add_num_button!(8);
                add_num_button!(9);
                if ui
                    .add_sized([40.0, 40.0], egui::Button::new("AC"))
                    .clicked()
                {
                    *self = Self::default();
                }
                ui.end_row();

                add_num_button!(4);
                add_num_button!(5);
                add_num_button!(6);
                if ui.add_sized([40.0, 40.0], egui::Button::new("/")).clicked() {
                    self.calc(Some(Operation::Div));
                }
                ui.end_row();

                add_num_button!(1);
                add_num_button!(2);
                add_num_button!(3);
                if ui.add_sized([40.0, 40.0], egui::Button::new("*")).clicked() {
                    self.calc(Some(Operation::Mul));
                }
                ui.end_row();

                add_num_button!(0);
                if ui.add_sized([40.0, 40.0], egui::Button::new("-")).clicked() {
                    self.calc(Some(Operation::Sub));
                }
                if ui.add_sized([40.0, 40.0], egui::Button::new("+")).clicked() {
                    self.calc(Some(Operation::Add));
                }
                if ui.add_sized([40.0, 40.0], egui::Button::new("=")).clicked() {
                    self.calc(None);
                };
            });
        });
    }
}
