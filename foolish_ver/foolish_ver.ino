int buzzerPin = 13;
int dotDelay = 250;
int buttonPin = 9;

boolean buttonState = LOW;

char message[] = "Attention1Attention1The9call9from9the9prospect9Lenina0There9is9fire9everywhere1";

char MorzeAlphabet[][5] = {
  {1, 2, 0, 0, 0}, //a
  {2, 1, 1, 1, 0}, //b
  {2, 1, 2, 1, 0}, //c
  {2, 1, 1, 0, 0}, //d
  {1, 0, 0, 0, 0}, //e
  {1, 1, 2, 1, 0}, //f
  {2, 2, 1, 0, 0}, //g
  {1, 1, 1, 1, 0}, //h
  {1, 1, 0, 0, 0}, //i
  {1, 2, 2, 2, 0}, //j
  {2, 1, 2, 0, 0}, //k
  {1, 2, 1, 1, 0}, //l
  {2, 2, 0, 0, 0}, //m
  {2, 1, 0, 0, 0}, //n
  {2, 2, 2, 0, 0}, //o
  {1, 2, 2, 1, 0}, //p
  {2, 2, 1, 2, 0}, //q
  {1, 2, 1, 0, 0}, //r
  {1, 1, 1, 0, 0}, //s
  {2, 0, 0, 0, 0}, //t
  {1, 1, 2, 0, 0}, //u
  {1, 1, 1, 2, 0}, //v
  {1, 2, 2, 0, 0}, //w
  {2, 1, 1, 2, 0}, //x
  {2, 1, 2, 2, 0}, //y
  {2, 2, 1, 1, 0}, //z
  {1, 2, 2, 2, 2}, //1
  {1, 1, 2, 2, 2}, //2
  {1, 1, 1, 2, 2}, //3
  {1, 1, 1, 1, 2}, //4
  {1, 1, 1, 1, 1}, //5
  {2, 1, 1, 1, 1}, //6
  {2, 2, 1, 1, 1}, //7
  {2, 2, 2, 1, 1}, //8
  {2, 2, 2, 2, 1}, //9
  {2, 2, 2, 2, 2}, //0
};

int standart = 3000;
int err = 4300;

void sound(int freq, int duration)
{
  tone(buzzerPin, freq);
  delay(duration);
  noTone(buzzerPin);
}

void dot()
{
  sound(standart, dotDelay);
  delay(dotDelay);
}

void dash()
{
  sound(standart, dotDelay * 3);
  delay(dotDelay);
}

void EndOfTheLetter()
{
  delay(dotDelay * 2);
}

void Error() {
  sound(err, dotDelay * 5);
  delay(dotDelay);
}

void playLetter(char letter)
{
  for (int i = 0; i < 5; i++)
  {
    if (MorzeAlphabet[letter - 97][i] == 2)
    {
      dash();
    }
    if (MorzeAlphabet[letter - 97][i] == 1)
    {
      dot();
    }
  }
}

volatile bool isEnd = false;

void functionA()
{
  buttonState = digitalRead(buttonPin);
  isEnd = false;
  if (digitalRead(buttonPin) == LOW) 
  {
    for (int i = 0; (i < 92) && (isEnd == false); i++)
    {
      playLetter(message[i]);
      if(digitalRead(buttonPin) == HIGH)
      {
        isEnd = true;
      }
    } 
  }
    else 
    {
      Error();
    }
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT);
}

void loop()
{
  functionA();
}

