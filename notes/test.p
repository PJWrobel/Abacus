#use std.pl as std


fn main(argc: int, argv: Vec<String>) -> Err {
    a := 3
    s1: String = "Hello"
    s2 := "World!" //&str
    s1.push(',')
      .push(' ')
      .append(s2)  //s1 = "Hello, World!"
    println(s1.split(' ')[0] + s2) // Hello, World!
}
