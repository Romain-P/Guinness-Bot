import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    application
    kotlin("jvm") version "1.3.50"
}

group = "com.guinness.bot"
version = "1.0-SNAPSHOT"

repositories {
    maven(url = "https://repo.spring.io/release")
    mavenCentral()
    maven(url = "https://jitpack.io")
}

val versions = mapOf(
    "ktor" to "1.2.4"
)

application {
    mainClassName = "MainKt"
}

dependencies {
    implementation(kotlin("stdlib-jdk8"))
    implementation(kotlin("reflect"))
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.3.2")
    implementation("io.projectreactor.netty:reactor-netty:0.9.0.RELEASE")
    implementation("com.github.kizitonwose:time:1.0.3")
    implementation("org.reflections:reflections:0.9.10")
}

tasks.withType<KotlinCompile> {
    kotlinOptions.jvmTarget = "1.8"
}