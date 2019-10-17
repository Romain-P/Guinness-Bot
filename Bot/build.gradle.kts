import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    application
    kotlin("jvm") version "1.3.41"
}

group = "com.guinness.bot"
version = "1.0-SNAPSHOT"

repositories {
    maven(url = "https://repo.spring.io/release")
    mavenCentral()
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
    implementation("io.projectreactor.netty:reactor-netty:0.9.0.RELEASE")
}

tasks.withType<KotlinCompile> {
    kotlinOptions.jvmTarget = "1.8"
}