package buildyourownkotlinlisp.parser

import kotlin.test.Test
import org.assertj.core.api.Assertions.assertThat

internal class ParseTreeTest {
    @Test
    fun `ParseTreeLeaf - it prints its name`() {
        val leaf = ParseTreeLeaf("element")
        assertThat(leaf.toString()).isEqualTo("Telement")
    }

    @Test
    fun `ParseTreeLeaf - it prints its name with the indentation`() {
        val leaf = ParseTreeLeaf("element")
        assertThat(leaf.multiLineString("  ")).isEqualTo("  T[element]\n")
    }

    @Test
    fun `ParseTreeNode - it prints its child on one line`() {
        val node = ParseTreeNode("node")
        node.child(ParseTreeLeaf("leaf"))
        assertThat(node.toString()).isEqualTo("Node(node) [Tleaf]")
    }

    @Test
    fun `ParseTreeNode - prints its child on multiple lines`() {
        val node = ParseTreeNode("node")
        node.child(ParseTreeLeaf("leaf"))
        assertThat(node.multiLineString()).isEqualTo("node\n T[leaf]\n")
    }
}
