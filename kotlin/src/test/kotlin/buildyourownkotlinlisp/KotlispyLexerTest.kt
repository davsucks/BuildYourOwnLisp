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
        assertEquals(listOf("NUMBER", "EOF"), tokens(lexerForCode("0")))
    }

    @Test
    fun `it should parse lots of numbers`() {
        assertEquals(listOf("NUMBER", "NUMBER", "NUMBER", "EOF"), tokens(lexerForCode("100 200 4324892")))
    }

    @Test
    fun `it should parse an operator`() {
        assertEquals(listOf("OPERATOR", "EOF"), tokens(lexerForCode("+")))
    }

    @Test
    fun `it should parse lots of operators`() {
        assertEquals(listOf("OPERATOR", "OPERATOR", "OPERATOR", "OPERATOR", "EOF"), tokens(lexerForCode("+ - * /")))
    }
}
