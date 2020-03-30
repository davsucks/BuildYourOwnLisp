package buildyourownkotlinlisp

import KotlispyLexer
import KotlispyParser
import buildyourownkotlinlisp.parser.toParseTree
import kotlin.test.Test
import org.antlr.v4.runtime.CharStreams
import org.antlr.v4.runtime.CommonTokenStream
import org.assertj.core.api.Assertions.assertThat

class KotlispyParserTest {
    private fun lexerForResource() =
        KotlispyLexer(CharStreams.fromString("+ 5 (* 2 2)"))

    private fun parseResource() =
        KotlispyParser(CommonTokenStream(lexerForResource()))

    @Test
    fun `it should parse the plus operator`() {
        val actual = toParseTree(parseResource().lispy()).multiLineString()
        assertThat(actual).contains("Operator")
        assertThat(actual).contains("T[+]")
    }

    @Test
    fun `it should parse an expression`() {
        val actual = toParseTree(parseResource().lispy()).multiLineString()
        assertThat(actual).contains("Expression")
    }
}
