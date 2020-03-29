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
    fun `it should parse a few expressions`() {
        val actual = toParseTree(parseResource().lispy()).multiLineString()
        val expected = """Lispy
 T[+]
 Expression
  T[5]
 Expression
  T[(]
  T[*]
  Expression
   T[2]
  Expression
   T[2]
  T[)]
 T[<EOF>]"""
        assertThat(actual).contains(expected)
    }
}
