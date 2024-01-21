# Chatfish

### Whisper

```bash
# download model
./models/download-ggml-model.sh tiny.en

brew install sdl2
# sudo apt-get install libsdl2-dev

brew install ffmpeg # (for ffplay)
# sudo apt-get install ffmpeg

make talk-llama
./talk-llama -mw ./models/ggml-tiny.en.bin -ml ../llama.cpp/models/llama-2-7b.Q4_0.gguf -p "Human" -t 8
```

Then modify `whisper.cpp/examples/talk-llama/speak` and `whisper.cpp/examples/talk-llama/eleven-labs.py`

### Text Generation

```bash
huggingface-cli download TheBloke/Llama-7B-GGUF llama-7b.Q4_K_M.g
guf --local-dir . --local-dir-use-symlinks False
```
