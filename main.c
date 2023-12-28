#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define EPS 0.001 //заданная точность вычислений
#define EPS_1 0.0001 //точность вычисления итеграла и точек пересечений
#define A_1 -3 //границы отрезков, которые описаны в отчете
#define B_1 0
#define C_1 3
#define SPLIT 20000 //начальное разбиение отрезка интегрирования

//функции f1, f2, f3
extern double f_1(double x);
extern double f_2(double x);
extern double f_3(double x);

//производные функций f1, f2, f3 соответсвенно
extern double df_1(double x);
extern double df_2(double x);
extern double df_3(double x);

static int iterations_cnt = 0; //подсчет итераций для вычисления точки пересечения
static int integral_iterations = 0; //подсчет итераций для вычисления интеграла

//выбор случая для методов хорд и касательных
int case_choice(double (*f)(double), double (*g)(double), double a, double b) {
    double F = (f((a + b) / 2) - g((a + b) / 2)) - ((f(a) - g(a) + f(b) - g(b))/2);
    //если F > 0, то график функции расположен выше хорды
    //Если функция возрастает и ее график ниже хорды, то случай 1
    //иначе случай 2 см в каждом методе свои
    
    //поскольку производные F'(x) и F''(x) на отрезке [a,b]
    //не меняют знак => достаточно сравнить F(a) с 0 (при F(a) < 0 функция возрастает)
    //где функция F(x) имеет вид f(x) - g(x)
    if (((f(a) - g(a)) * F) > 0) {
        return 1;
    } else {
        return 0;
    }
}

//метод хорд
double chord_method(double (*f)(double), double (*g)(double), double a, double b, double *A, double *B) {
    int case_chord = case_choice(f, g, a, b);

    double F_a = f(a) - g(a);
    double F_b = f(b) - g(b);
    double new_coord = (a * F_b - b * F_a) / (F_b - F_a);
    
    //если случай 1: первая и вторая производные функции F(x) = f(x) - g(x) имеют одинаковый знак
    //выбираем отрезок [new_coord, b]
    //иначе [a, new_coord]

    if (case_chord) {
        *A = new_coord;
    } else {
        *B = new_coord;
    }
}

//метод касательных
double tangent_method(double (*f)(double), double (*g)(double), double (*df)(double), double (*dg)(double), double a, double b, double *A, double *B) {
    //если случай 1: первая и вторая производные функции F(x) = f(x) - g(x) имеют одинаковый знак
    //то проводится касательная к кривой F(x) = f(x) - g(x) в точке (b, F(b))
    //иначе в точке (a, F(a))
    double d = a;
    int case_tangent = case_choice(f, g, a, b);

    if (case_tangent) {
        d = b;
    }

    double new_coord = d - (f(d) - g(d)) / (df(d) - dg(d)); //точка пересечения этой касательной с осью абсцисс

    if (case_tangent) {
        *B = new_coord;
    } else {
        *A = new_coord;
    }
}

//вызов комбинированного метода нахождения абсцисс точек пересечения функций f1-f2, f2-f3, f3-f1
double root(double (*f)(double), double (*g)(double), double (*df)(double), double (*dg)(double), double a, double b, double eps) {
    if ((f(a) - g(a)) * (f(b) - g(b)) > 0) {
        printf("Error: too big segment or no roots please try with smaller one!");
        return 1;
    }

    if (b - a < eps) {
        return (a + b) / 2;
    }

    double A = a; //приближаемые к точке пересечения границы
    double B = b;
    
    tangent_method(f, g, df, dg, a, b, &A, &B);
    chord_method(f, g, a, b, &A, &B);
    
    iterations_cnt++;
    
    return root(f, g, df, dg, A, B, eps);
}

//функция подсчета интеграла функции f на отрезке [a,b] методом прямоугольников
double integral(double (*f)(double), double a, double b, double eps) {
    double diff = fabs(b - a) / SPLIT;
    double integ_1 = 0, integ_2 = 0;
    double x = a + diff / 2;
    
    for (int i = 0; i < SPLIT; i++) {
        integ_2 += f(x) * diff;
        x += diff;
    }
    
    int n = SPLIT;
    
    while (fabs(integ_2 - integ_1) / 3 >= eps) {
        if ((integral_iterations) > 7) {
            printf("\nI cant calculate this integral with this eps or answer is +-infinity please try wwith a smaller segment\n");
            return integ_2;
        }
        
    	integral_iterations++;
        integ_1 = integ_2;
        integ_2 = 0;
        n *= 2;
        diff /= 2;
        x = a + diff / 2;
		
        for (int i = 0; i < n; i++) {
            integ_2 += f(x) * diff;
            x += diff;
        }
    }
	
    return integ_2;
}

//печать функций
void functions() {
    printf("Functions:\n f1 = 2^x + 1;\n f2 = x^5;\n f3 = (1 - x) / 3;\n");
}

//вывод всех ключей для работы с программой
void help() {
    printf("-help - for more information\n");
    printf("-root - to see intersection points of f1, f2, f3\n");
    printf("-iterations - to see the number of iterations for calculating the intersection points\n");
    printf("-functions - to see functions f1, f2, f3\n");
    printf("-testintegral <function: f1/f2/f3> <left border> <right border> <accuracy(eps)> - to test the correctness of calculating integral\n");
    printf("-testroot -  <1st function: f1/f2/f3> <2nd function: f1/f2/f3> <left border> <right border> <accuracy(eps)> to test the correctness of calculating the intersection points\n");
    
}

//печать точек пересечений функций
void abscissas() {
    printf("Functions: f1 = 2^x + 1; f2 = x^5; f3 = (1 - x) / 3 \n\n");
    printf("intersection point of functions f1 and f2: %lf\n", root(f_1,f_2,df_1,df_2, B_1, C_1, EPS_1));
    printf("intersection point of functions f2 and f3: %lf\n", root(f_2,f_3,df_2,df_3, B_1, C_1, EPS_1));
    printf("intersection point of functions f1 and f3: %lf\n", root(f_1,f_3,df_1,df_3, A_1, B_1, EPS_1));
}

//печать количества итераций для подсчета точек пересечений
void iterations() {
    iterations_cnt = 0;
    printf("Functions: f1 = 2^x + 1; f2 = x^5; f3 = (1 - x) / 3 \n\n");
    printf("intersection point of functions f1 and f2: %lf\n", root(f_1,f_2,df_1,df_2, B_1, C_1, EPS_1));
    printf("iterations to calculate this coords: %d\n\n", iterations_cnt);
    iterations_cnt = 0;
    printf("intersection point of functions f2 and f3: %lf\n", root(f_2,f_3,df_2,df_3, B_1, C_1, EPS_1));
    printf("iterations to calculate this coords: %d\n\n", iterations_cnt);
    iterations_cnt = 0;
    printf("intersection point of functions f1 and f3: %lf\n", root(f_1,f_3,df_1,df_3, A_1, B_1, EPS_1));
    printf("iterations to calculate this coords: %d\n", iterations_cnt);
    iterations_cnt = 0;
}

//функция проверки корректности работы функции итегрирования
void test_integral(char* function, char* x, char* y, char* eps_c) {
    double a = atof(x);
    double b = atof(y);
    double eps = atof(eps_c);
    
    if (eps <= 0) {
        printf("\nEPS in -testintegral must be number > 0\n");
        
        return;
    }
    
    if (!strcmp(function, "f1")) {
        printf("Your function is: 2^x + 1\n");
    	printf("Tested Integral is: %lf\n", integral(f_1, a, b, eps));
    } else if (!strcmp(function, "f2")) {
        printf("Your function is: x^5\n");
    	printf("Tested Integral is: %lf\n", integral(f_2, a, b, eps));
    } else if (!strcmp(function, "f3")) {
        printf("Your function is: (1-x)/3\n");
    	printf("Tested Integral is: %lf\n", integral(f_3, a, b, eps));
    	
    } else {
        printf("Wrong function in -testintegral please check -help");
    }
        
    integral_iterations = 0;
}

//функция проверки корректности работы функции нахождения пересечений кривых
void test_root(char* function_1, char* function_2, char* x, char* y, char* eps_c) {
    double eps = atof(eps_c);
    
    if (eps <= 0) {
        printf("\nEPS in -testroot must be number > 0\n");
        
        return;
    }
    
    double a = atof(x);
    double b = atof(y);
    
    if ((!strcmp(function_1, "f1") && !strcmp(function_2, "f2")) || (!strcmp(function_1, "f2") && !strcmp(function_2, "f1"))) {
        if (root(f_1, f_2, df_1, df_2, a, b, eps) != 1) {
            printf("The tested point is: %lf\n", root(f_1, f_2, df_1, df_2, a, b, eps));
        }
    } else if ((!strcmp(function_1, "f1") && !strcmp(function_2, "f3")) || (!strcmp(function_1, "f3") && !strcmp(function_2, "f1"))) {
        if (root(f_1, f_3, df_1, df_3, a, b, eps) != 1) {
            printf("The tested point is: %lf\n", root(f_1, f_3, df_1, df_3, a, b, eps));
        }
    } else if ((!strcmp(function_1, "f2") && !strcmp(function_2, "f3")) || (!strcmp(function_1, "f3") && !strcmp(function_2, "f2"))) {
        if (root(f_2, f_3, df_2, df_3, a, b, eps) != 1) {
            printf("The tested point is: %lf\n", root(f_2, f_3, df_2, df_3, a, b, eps));
        }
    } else {
    	printf("Wrong functions in -testroot please check -help");
    }
}

int main(int argc, char* argv[]) {
    printf("Type -help to see more information\n\n");
    
    for (int i = 1; i < argc; i++){
    	if (!strcmp(argv[i], "-help")) {
            help();
    	} else if (!strcmp(argv[i], "-iterations")) {
            iterations();
        } else if (!strcmp(argv[i], "-root")) {
            abscissas();
    	} else if (!strcmp(argv[i], "-functions")) {
            functions();
    	} else if (!strcmp(argv[i], "-testintegral")) {
    	    putchar('\n');
            
    	    if (!argv[i + 1] || !argv[i + 2] || !argv[i + 3] || !argv[i + 4]) {
    	        printf("Not enough params in -testintegral please check -help\n");
    	    } else {
                test_integral(argv[i + 1], argv[i + 2], argv[i + 3], argv[i + 4]);
            }
            
            i += 1;
            putchar('\n');
        } else if (!strcmp(argv[i], "-testroot")) {
            putchar('\n');
            int flag = 0;
            
            if (!argv[i + 1] || !argv[i + 2] || !argv[i + 3] || !argv[i + 4] || !argv[i + 5]) {
    	         printf("Not enough params in -testroot please check -help\n");
    	    
    	        flag = 1;
    	    }
    	
    	    if (flag == 0 && !strcmp(argv[i + 1], argv[i + 2])) {
    	        printf("Please type different functions in -testroot -_-");
    	    
    	        flag = 1;
    	    }
            
            if (!flag) {
                test_root(argv[i + 1], argv[i + 2], argv[i + 3], argv[i + 4], argv[i + 5]);
                i += 1;
                putchar('\n');
            }
        } 
    }
    
    double root_12 = root(f_1,f_2,df_1,df_2, B_1, C_1, EPS_1);
    double root_23 = root(f_2,f_3,df_2,df_3, B_1, C_1, EPS_1);
    double root_13 = root(f_1,f_3,df_1,df_3, A_1, B_1, EPS_1);
    
    double integral_1_p13_p12 = integral(f_1, root_13, root_12, EPS_1);
    int integral_iterations_1 = integral_iterations;
    integral_iterations = 0;
    double integral_3_p13_p23 = integral(f_3, root_13, root_23, EPS_1);
    int integral_iterations_2 = integral_iterations;
    integral_iterations = 0;
    double integral_2_p12_p32 = integral(f_2, root_23, root_12, EPS_1);
    int integral_iterations_3 = integral_iterations;
    integral_iterations = 0;
    
    double answer = integral_1_p13_p12 - integral_3_p13_p23 - integral_2_p12_p32;
    
    printf("\n-----------------------------------------");
    printf("\nAnswer:\n");
    printf("Square of the area between the functions: %lf\n", answer);
    printf("Iterations for calculating integrals: %d, %d, %d\n", integral_iterations_1, integral_iterations_2, integral_iterations_3);
    
    return 0;
}
