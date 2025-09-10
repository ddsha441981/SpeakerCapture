compile
cd "/media/deendayal/personal/My Data/Development/SpeakerCaptureWithSimplePA" && g++ main.cpp SimplePA/SimplePA.cpp -I./SimplePA -lpulse-simple -lpulse -pthread -std=c++17 -o SpeakerCapture


run code
./SpeakerCapture
