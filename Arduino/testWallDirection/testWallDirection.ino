enum Direction{
  North = 0x01,
  East = 0x04,
  South = 0x02,
  West = 0x08
};

void setup() {
  Serial.begin(115200);
  Serial.println("TEST");
}

void loop() {
  if(Serial.read() >0) {
    callYourMama();
  }
}

void updateTileCondition2(Direction dir, boolean FWall, boolean LWall, boolean RWall){
  char walls = 0x00;
  if (FWall) walls+=1;
  if (LWall) walls +=8;
  if (RWall) walls +=2;
Serial.println(walls, HEX);
  for(int i=3; i>0; i--){
    if (dir>>i){
      walls = ((walls<<i) % 16) | (walls>>(4-i));
      break;
    }
  }
Serial.println(walls, HEX);
  if (walls&8) Serial.println("West Wall");
  if (walls&4) Serial.println("South Wall");
  if (walls&2) Serial.println("East Wall");
  if (walls&1) Serial.println("North Wall");
}

void callYourMama() {
  updateTileCondition2(West, true, false, true);
}

void updateTileCondition(Direction dir, boolean FWall, boolean LWall, boolean RWall){
  if(dir==North) {    // Heading is north
    if(FWall) {
      // north wall
      Serial.println("north Wall");
    }
    if(LWall) {
      // west wall
      Serial.println("West Wall");
    }
    if(RWall) {
      // east wall
      Serial.println("East Wall");
    }
  }else if(dir==East) {    // heading east
    if(FWall) {
      // East wall
      Serial.println("East Wall");
    }
    if(LWall) {
      // north wall
      Serial.println("north Wall");
    }
    if(RWall) {
      // south wall
      Serial.println("South Wall");
    }
  } else if(dir==South) {   // heading south
    if(FWall) {
      // South wall
      Serial.println("South Wall");
    }
    if(LWall) {
      // East wall
      Serial.println("East Wall");
    }
    if(RWall) {
      // West wall
      Serial.println("West Wall");
    }
  } else if(dir==West) {   // heading west
    if(FWall) {
      // West wall
      Serial.println("West Wall");
    }
    if(LWall) {
      // South wall
      Serial.println("South Wall");
    }
    if(RWall) {
      // North wall
      Serial.println("north Wall");
    }
  }
}

