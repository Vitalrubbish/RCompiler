fn make_origin() -> Point {
    Point { x: 0.0, y: 0.0 }
}

// 然后定义 `Point` 结构体
struct Point {
    x: f64,
    y: f64,
}

fn main() {
    let origin = make_origin();
    println!("Origin: ({}, {})", origin.x, origin.y);
}