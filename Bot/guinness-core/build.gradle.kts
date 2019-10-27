plugins {
    application
}

application {
    mainClassName = "MainKt"
}

dependencies {
    implementation(kotlin("stdlib-jdk8"))
    implementation(kotlin("reflect"))
    implementation(kotlin("script-runtime"))
    implementation(kotlin("compiler-embeddable"))
    implementation(kotlin("script-util"))
    implementation(kotlin("scripting-compiler-embeddable"))
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.3.2")
    implementation("io.projectreactor.netty:reactor-netty:0.9.0.RELEASE")
    implementation("com.github.kizitonwose:time:1.0.3")
    implementation("org.reflections:reflections:0.9.10")
    implementation("no.tornado:tornadofx:1.7.19")
    implementation(fileTree("lib"))
    implementation(project(":guinness-api"))
}