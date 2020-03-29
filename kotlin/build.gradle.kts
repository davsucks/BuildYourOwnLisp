plugins {
    // Apply the Kotlin JVM plugin to add support for Kotlin.
    id("org.jetbrains.kotlin.jvm") version "1.3.70"

    // Apply the application plugin to add support for building a CLI application.
    application
    // antlr is a lexing and parsing library
    antlr

    // ktlint is a kotlin linting library
    id("org.jlleitschuh.gradle.ktlint") version "9.2.1"
}

repositories {
    // Use jcenter for resolving dependencies.
    // You can declare any Maven/Ivy/file repository here.
    jcenter()
    mavenCentral()
}

dependencies {
    antlr("org.antlr:antlr4:4.8")

    // Align versions of all Kotlin components
    implementation(platform("org.jetbrains.kotlin:kotlin-bom"))
    // Use the Kotlin JDK 8 standard library.
    implementation("org.jetbrains.kotlin:kotlin-stdlib-jdk8")

    // Use the Kotlin test library.
    testImplementation("org.jetbrains.kotlin:kotlin-test")
    // Use the Kotlin JUnit integration.
    testImplementation("org.jetbrains.kotlin:kotlin-test-junit")
    // fluent assertions
    testImplementation("org.assertj:assertj-core:3.15.0")
}

application {
    // Define the main class for the application.
    mainClassName = "buildyourownkotlinlisp.AppKt"
}

val run by tasks.getting(JavaExec::class) {
    standardInput = System.`in`
}
