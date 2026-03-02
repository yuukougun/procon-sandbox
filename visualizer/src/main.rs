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

#[derive(Default, Debug)]
struct MyApp {
    // ユーザーに表示される値
    display: i64,
    // 入力値
    input: i64,
    //　値の合計
    stack: i64,
    operation: Option<Operation>,
}

#[derive(Debug)]
enum Operation {
    Add,
    Sub,
    Mul,
    Div,
}

impl eframe::App for MyApp {
    fn update(&mut self, ctx: &egui::Context, _: &mut eframe::Frame) {
        println!("{:?}", self);
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.label(egui::RichText::new(format!("{}", self.display)).size(40.0));
            egui::Grid::new("keyboard").show(ui, |ui| {
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
                    // リセット
                    *self = Self::default();
                }
                ui.end_row();
                
                add_num_button!(4);
                add_num_button!(5);
                add_num_button!(6);
                if ui.add_sized([40.0, 40.0], egui::Button::new("/")).clicked() {
                    todo!()
                }
                ui.end_row();
                
                add_num_button!(1);
                add_num_button!(2);
                add_num_button!(3);
                if ui.add_sized([40.0, 40.0], egui::Button::new("*")).clicked() {
                    todo!()
                }
                ui.end_row();
                
                add_num_button!(0);
                if ui.add_sized([40.0, 40.0], egui::Button::new("-")).clicked() {
                    todo!()
                }
                if ui.add_sized([40.0, 40.0], egui::Button::new("+")).clicked() {
                    match self.operation.replace(Operation::Add) {
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
                if ui.add_sized([40.0, 40.0], egui::Button::new("=")).clicked() {
                    if let Some(op) = self.operation.take() {
                        match op {
                            Operation::Add => self.stack += self.input,
                            Operation::Sub => self.stack -= self.input,
                            Operation::Mul => self.stack *= self.input,
                            Operation::Div => self.stack /= self.input,
                        }
                    } else {
                        self.stack = self.display;
                    }
                    self.display = self.stack;
                    self.input = 0;
                };
            });
        });
    }
}
