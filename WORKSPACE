load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

whisper_hash="a057fb23891c9e5cc11b4ffa7a3b5ce4fdd01d78"
git_repository(
  name = "whisper",
  commit = whisper_hash,
  remote = "https://github.com/tenstorrent/whisper.git",
)

git_repository(
    name = "googletest",
    remote = "https://github.com/google/googletest",
    #commit = "1b18723e874b256c1e39378c6774a90701d70f7a"
    tag = "release-1.11.0",
)
