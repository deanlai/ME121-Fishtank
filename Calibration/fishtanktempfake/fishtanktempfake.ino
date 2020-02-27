void setup() {
  Serial.begin(9600);

}

void loop() {
    
    const float tc1 = 0.00015464668;
    const float tc2 = -0.068341165;
    const float tc3 = 18.217030;
    float systemTemp;
    float esystemp;

    const float ftc1 = 1.5464668e-04;
    const float ftc2 = -6.8341165e-02;
    const float ftc3 = 1.8217030e+01;
   
    systemTemp = findTemp(600, tc1, tc2, tc3); 
    esystemp = findTemp(600, ftc1, ftc2, ftc3);
    Serial.print(" ");
    Serial.println(systemTemp);
    Serial.println(tc1, 8);
    Serial.println(esystemp);
    Serial.println(ftc1, 8);
    delay(3000);
}


float findTemp(int reading, float c1, float c2, float c3) {
   return evaluate2ndPolynomial(reading, c1, c2, c3);
}
float evaluate2ndPolynomial(int x, float c1, float c2, float c3) {
    //Serial.println(c1,8);
    //Serial.println(c2, 8);
    //Serial.println(c3, 8);
    //evaluates y = c1*x^2 + c2*x + 3 and returns y
    return c1*x*x + c2*x + c3;
}
