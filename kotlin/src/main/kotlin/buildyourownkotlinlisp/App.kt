package buildyourownkotlinlisp

class App {
    val greeting: String
        get() {
            return "Hello world."
        }
}

fun main(args: Array<String>) {
    println("Lispy Version 0.0.0.0.1")
    println("Press Ctrl+c or type 'exit' to Exit\n")

    while (true) {
        print("lispy> ")

        val input = readLine()!!
        println("no, you're a $input")
    }
}
