import org.jetbrains.kotlin.gradle.tasks.KotlinCompile

plugins {
    application
    kotlin("jvm") version "1.3.41"
}

group = "com.guinness.bot"
version = "1.0-SNAPSHOT"

repositories {
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
    implementation("io.ktor:ktor-server-core:${versions["ktor"]}")
    implementation("io.ktor:ktor-server-netty:${versions["ktor"]}")
}

tasks.withType<KotlinCompile> {
    kotlinOptions.jvmTarget = "1.8"
}
