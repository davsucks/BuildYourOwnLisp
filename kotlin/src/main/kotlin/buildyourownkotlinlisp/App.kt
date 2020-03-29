package buildyourownkotlinlisp

import KotlispyLexer
import KotlispyParser
import buildyourownkotlinlisp.parser.toParseTree
import org.antlr.v4.runtime.CharStreams
import org.antlr.v4.runtime.CommonTokenStream

class App {
    val greeting: String
        get() {
            return "Hello world."
        }
}

fun main(args: Array<String>) {
    println("Kotlin Based Lispy Version 0.0.0.0.1")
    println("Press Ctrl+c or type 'exit' to Exit\n")

    while (true) {
        print("kotlispy> ")

        val input = readLine()!!
        if (input == "exit") break
        val parsedTree =
            toParseTree(KotlispyParser(CommonTokenStream(KotlispyLexer(CharStreams.fromString(input)))).lispy())
        println(parsedTree.multiLineString())
    }
}
