from gtts import gTTS 
import os

g = 0xedb88320

def f(a):
    return (a>>1)^g

if __name__ == "__main__":
    #tts = gTTS(text="robot lady", lang='en')
    #tts.save("tmp.mp3")
    #os.system("mpg321 tmp.mp3")
    #os.remove("tmp.mp3")

    r = 0xffffffff
    inp = chr(127)
    m = 0x6dc1
    c = int(1)
    print(hex(((g>>3)^g)%m))
    s = "".join([inp]*c)
    s = input()
    tt = 0xedb88320
    print(bin(tt), bin(r))
    print(hex(f(r) % 0x6dc1))
    print(s)
    for c in bytearray(s, 'ascii'):
        for i in range(8):
            r = r >> 1 ^ ((r^c>>i)&1)*0xedb88320
    print(hex(r))
    print(['error', 'success'][not r%0x6dc1])

