# AudioDSP
DSP Algorithm code for Audio Plugin <br>
**[blog](https://blog.naver.com/rbdus0715/224140506531)**: documented what I learn on my blog<br>

### basic class diagram
<img width="773" height="305" alt="image (1)" src="https://github.com/user-attachments/assets/3605fee5-7a04-4d72-af09-877812e3a838" />


## requirements
- CMAKE
- SDL2

## build & run
```bash
mkdir -p build && cd build
cmake ..
make
```

```bash
#/build
./AudioDSP
```
- place test.wav in assets/audio/ directory

## Reference
base code: https://github.com/BennyQBD/AudioTutorial <br>
reference book: [Hack Audio](https://www.oreilly.com/library/view/hack-audio/9781351018449/) and [Audio Effect](https://product.kyobobook.co.kr/detail/S000028477616)
