package buildyourownkotlinlisp

import KotlispyLexer
import kotlin.test.Test
import kotlin.test.assertEquals
import org.antlr.v4.parse.ANTLRLexer.WS
import org.antlr.v4.runtime.CharStreams

class KotlispyLexerTest {

    private fun lexerForCode(code: String) = KotlispyLexer(
        CharStreams.fromString(code)
    )

    private fun tokens(lexer: KotlispyLexer): List<String> {
        val tokens = mutableListOf<String>()
        do {
            val t = lexer.nextToken()
            when (t.type) {
                -1 -> tokens.add("EOF")
                else -> if (t.type != WS) tokens.add(lexer.ruleNames[t.type - 1])
            }
        } while (t.type != -1)
        return tokens
    }

    @Test
    fun `it should parse a number`() {
        assertEquals(listOf("DIGIT", "EOF"), tokens(lexerForCode("0")))
    }

    @Test
    fun `it should parse lots of numbers`() {
        assertEquals(listOf("DIGIT", "DIGIT", "DIGIT", "EOF"), tokens(lexerForCode("100")))
    }

    @Test
    fun `it should parse an operator`() {
        assertEquals(listOf("PLUS", "EOF"), tokens(lexerForCode("+")))
    }

    @Test
    fun `it should parse lots of operators`() {
        assertEquals(listOf("PLUS", "MINUS", "MULT", "DIV", "EOF"), tokens(lexerForCode("+ - * /")))
    }
}
