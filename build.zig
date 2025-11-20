const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const raylib = b.dependency("raylib", .{});
    const raygui = b.dependency("raygui", .{});
    const portaudio = b.dependency("portaudio", .{});
    const portaudio_headers = b.dependency("portaudio_headers", .{});

    // portaudio
    const exe = b.addExecutable(.{
        .name = "harald_microphone_challange",
        .root_module = b.createModule(.{
            .target = target,
            .optimize = optimize,
            .link_libc = true,
        }),
    });
    exe.addCSourceFiles(.{
        .root = b.path("src/"),
        .files = &.{
            "audio.c",
            "background.c",
            "collider.c",
            "game.c",
            "main.c",
            "renderer.c",
            "rope.c",
        },
    });

    exe.addIncludePath(b.path("src/"));
    exe.addIncludePath(b.path("include/"));

    // raylib
    exe.addIncludePath(raylib.path("src/"));
    // raygui
    exe.addIncludePath(raygui.path("src/"));
    // portaudio
    exe.addIncludePath(portaudio_headers.path("include/"));
    exe.addIncludePath(portaudio_headers.path("src/common/"));

    exe.linkLibrary(raylib.artifact("raylib"));
    exe.linkLibrary(portaudio.artifact("portaudio"));

    b.installArtifact(exe);

    const run_step = b.step("run", "Run the app");

    const run_cmd = b.addRunArtifact(exe);
    run_step.dependOn(&run_cmd.step);
    run_cmd.step.dependOn(b.getInstallStep());
    if (b.args) |args| run_cmd.addArgs(args);
}
