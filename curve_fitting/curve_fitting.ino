#include <math.h>

#define PIN_IR A0

#define MAX_EXP 5

int n = 100;
int exponent = 2;

double data[7];

void setup() {

  Serial.begin(1000000);

  Serial.print("How many values would you check?");
  while (Serial.available() == 0) ;
  n = Serial.parseInt();
  Serial.println();
  while (Serial.available()) {
  Serial.read();
  }
 
  Serial.print("Choose the exponent of the equation (0 ~ 3)");
  while (Serial.available() == 0) ;
  exponent = Serial.parseInt();
  Serial.println();

  while (Serial.available()) {
  Serial.read();
  }
  
}

void loop() {
    unsigned int filtered;

    for(int i = 0; i < 7; i++) {
      waitLine();

      filtered = ir_sensor_filtered(n, 0.5, 0);
      Serial.print("FLT:"); Serial.print(filtered); Serial.println();
      data[i] = filtered;
    }

    double theta[MAX_EXP + 1];
    curve_fitting(theta);

    Serial.print("Equation :");

    for(int i = 0; i < exponent + 1; i++){
      Serial.print(" ");
      Serial.print(theta[i], 6);
      Serial.print("* x ^ ");
      Serial.print(i);
      if(i < exponent) Serial.print(" + ");  
    }

    Serial.println();
    
}

int compare(const void *a, const void *b) {
  return (*(unsigned int *)a - *(unsigned int *)b);
}

unsigned int ir_sensor_filtered(unsigned int n, float position, int verbose)
{
  
  unsigned int *ir_val, ret_val;
  unsigned int start_time;
 
  if (verbose >= 2)
    start_time = millis(); 

  if ((n == 0) || (n > 100) || (position < 0.0) || (position > 1))
    return 0;
    
  if (position == 1.0)
    position = 0.999;

  if (verbose == 1) {
    Serial.print("n: "); Serial.print(n);
    Serial.print(", position: "); Serial.print(position); 
    Serial.print(", ret_idx: ");  Serial.println((unsigned int)(n * position)); 
  }

  ir_val = (unsigned int *)malloc(sizeof(unsigned int) * n);
  if (ir_val == NULL)
    return 0;

  if (verbose == 1)
    Serial.print("IR:");
  
  for (int i = 0; i < n; i++) {
    ir_val[i] = analogRead(PIN_IR);
    if (verbose == 1) {
        Serial.print(" ");
        Serial.print(ir_val[i]);
    }
  }

  if (verbose == 1)
    Serial.print  ("  => ");

  qsort(ir_val, n, sizeof(unsigned int), compare);
  ret_val = ir_val[(unsigned int)(n * position)];

  if (verbose == 1) {
    for (int i = 0; i < n; i++) {
        Serial.print(" ");
        Serial.print(ir_val[i]);
    }
    Serial.print(" :: ");
    Serial.println(ret_val);
  }
  free(ir_val);

  if (verbose >= 2) {
    Serial.print("Elapsed time: "); Serial.print(millis() - start_time); Serial.println("ms");
  }
  
  return ret_val;
}

void curve_fitting (double theta[]) {
  double matrix[7][MAX_EXP + 1];

  for(int i = 0; i < 7; i++) {
    for(int j = 0; j < exponent + 1; j++)
      matrix[i][j] = pow(data[i], j);
  }

  double result[7] = {0, 50, 100, 150, 200, 250, 300};

  double transpose[MAX_EXP + 1][7];
  for(int i = 0; i < exponent + 1; i++) {
    for(int j = 0; j < 7; j++) {
      transpose[i][j] = matrix[j][i];
    }
  }

  double A[MAX_EXP + 1][MAX_EXP + 1];
  for(int i = 0; i < exponent + 1; i++){
    for(int j = 0; j < exponent + 1; j++){
      A[i][j] = 0.0;
      for(int k = 0; k < 7; k++) {
        A[i][j] += transpose[i][k] * matrix[k][j];
      } 
    }
  }
  
  double B[MAX_EXP + 1];
  for(int i = 0; i < exponent + 1; i++){
    B[i] = 0.0;
    for(int j = 0; j < 7; j++) {
      B[i] += transpose[i][j] * result[j];
    }
  }

  double cal[MAX_EXP + 1][MAX_EXP + 2];
  for(int i = 0; i < exponent + 1; i++) {
    for(int j = 0; j < exponent + 1; j++) {
      cal[i][j] = A[i][j];
    }
    cal[i][exponent + 1] = B[i];
  }

  for(int i = 0; i < exponent; i++) {
    double pivot = cal[i][i];

    for(int j = i + 1; j < exponent + 1; j++) {
      double factor = cal[j][i] / pivot;
      for(int k = i; k < exponent + 2; k++) {
        cal[j][k] -= factor * cal[i][k];
      }
    }

  }
  for (int i = exponent; i >= 0; i--) {
    double sum = cal[i][exponent + 1];
    for(int j = i + 1; j < exponent + 1; j++) {
      sum -= cal[i][j] * theta[j];
    }

    theta[i] = sum / cal[i][i];
  }
}

void waitLine() {
  char c;

  while (Serial.available() == 0);

  String dump = Serial.readStringUntil('\n');
   
  
}
