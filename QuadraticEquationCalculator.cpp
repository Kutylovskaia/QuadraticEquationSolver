#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "color.h"
#include "color.h"

typedef struct {
    double a, b, c;
} CoeffCase;

typedef struct {
    CoeffCase CoeffEquation;
    int n_roots_ref;
    double x1_ref, x2_ref;
} TestCase;

enum ProgramModes {
                PRACTICE = 0,
                SOLUTION = 1,
                WRONG_CHOICE = 2
                };

enum Roots {NO_ROOTS = 0,
            ONE_ROOT = 1,
            TWO_ROOTS = 2,
            INFINITY_ROOTS = 3
            };

enum WrongCoefficients {WRONG_FIRST_COEFFICIENT = 0,
                        WRONG_SECOND_COEFFICIENT  = 1,
                        WRONG_THIRD_COEFFICIENT = 2,
                        WRONG_SYMBOLS_AFTER = 3,
                        NO_ERROR = 4
                        };

enum Signs {SING_MINUS_WHITESPACE = 0,
            SIGN_PLUS_MINUS = 1,
            SING_WHITESPACE_MINUS = 2
            };

const double EPSILON = 1e-6;
const int NUMBER_MANDATORY_SCREENING_TESTS = 9;

// объявление функций
int RootsFind (const CoeffCase* CoeffEquation, double* x1_ptr, double* x2_ptr, bool solution_need);
int LinearEquation (const CoeffCase* CoeffEquation, double* x1_ptr, bool solution_need);
void PrintAnswer (int k_roots, double x1, double x2);
int EnterVariables (CoeffCase* CoeffEquation);
void PracticeSolvingEquations (void);
void SolveEquation (CoeffCase* CoeffEquation);

// работа с числами
int IsEqual (double a, double b);
char GetOneSign (double x, int execution_option);

// поиск ошибок
void ErrorNotification (int error);
void CheckCorrectResponses (const CoeffCase* CoeffEquation, double x1, double x2, int k_roots);
bool CheckEnum (void);
void RunOneTest (TestCase Test, int number_test);
void RunTests (void);
void SkipUnuselessCharacters (void);

// функции вопросники
bool QuestionContinue (void);
bool QuestionSolution (void);
int QuestionPracticeOrSolution (void);

// функции напечатать
void PrintRight (void);
void PrintGaveWrongAnswer (void);
void UpliftingMoodAfterCorrectAnswers ();
void UpliftingMoodAfterWrongAnswers (double a, double b, double c, double* x1_ptr, double* x2_ptr);

int main(int argc, char *argv[]) {
    int comand_line_run_test = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp (argv[i], "-tests") == 0) {
            comand_line_run_test = 1;
        }
    }

    if (comand_line_run_test)
        RunTests ();

    if (CheckEnum ())
        return 0;

    CoeffCase CoeffEquation = {.a = NAN, .b = NAN, .c = NAN};

    int program_mode = QuestionPracticeOrSolution ();

    switch (program_mode) {
        case PRACTICE:
            PracticeSolvingEquations ();
            break;

        case SOLUTION:
            SolveEquation (&CoeffEquation);
            break;

        default:
            printf (BRIGHT_RED"Restart the program\n");
    }

    printf (BRIGHT_MAGENTA"See you!\n");

    return 0;
}

int RootsFind (const CoeffCase* CoeffEquation, double* x1_ptr, double* x2_ptr, bool solution_need) {
    // проверяем корректность адресов
    assert (CoeffEquation != NULL);
    assert (x1_ptr != NULL);
    assert (x2_ptr != NULL);
    assert (x1_ptr != x2_ptr);

    if (IsEqual (CoeffEquation->a, 0))

        return LinearEquation (CoeffEquation, x1_ptr, solution_need);

    else {

        /* считаем дискриминант*/
        double d = CoeffEquation->b * CoeffEquation->b - 4 * CoeffEquation->a * CoeffEquation->c;
        double d_sqrt = 0;

        if (d > -EPSILON) {
            d_sqrt = sqrt (d);

            if (solution_need)
                printf (BRIGHT_YELLOW"%lg*x^2 %c %lg*x %c %lg = 0\nD = %lg\nx = (%c%lg +- %lg) / (%c2*%lg)\n",
                         CoeffEquation->a,
                         GetOneSign (CoeffEquation->b, SIGN_PLUS_MINUS),        fabs(CoeffEquation->b),
                         GetOneSign (CoeffEquation->c, SIGN_PLUS_MINUS),        fabs(CoeffEquation->c), d,
                         GetOneSign (CoeffEquation->b, SING_MINUS_WHITESPACE),  fabs(CoeffEquation->b), d_sqrt,
                         GetOneSign (CoeffEquation->a, SING_WHITESPACE_MINUS),  fabs(CoeffEquation->a));

            if (IsEqual (d, 0)) {
                *x1_ptr = (-CoeffEquation->b + d_sqrt) / (2 * CoeffEquation->a);
                return ONE_ROOT;
            }

            else if (d > 0) {
                *x1_ptr = (-CoeffEquation->b + d_sqrt) / (2 * CoeffEquation->a);
                *x2_ptr = (-CoeffEquation->b - d_sqrt) / (2 * CoeffEquation->a);
                return TWO_ROOTS;
            }
        }

        else {
            if (solution_need)
                printf (BRIGHT_YELLOW"D = %lg <0\n", d);
            return NO_ROOTS;
        }
     }
}

int LinearEquation (const CoeffCase* CoeffEquation, double* x1_ptr, bool solution_need) {
    // проверяем корректность адресов
    assert (x1_ptr != NULL);
    assert (CoeffEquation != NULL);

    /* все коэффициенты нули */
    if (IsEqual (CoeffEquation->b, 0) && IsEqual (CoeffEquation->c, 0)) {

        if (solution_need)
            printf (BRIGHT_YELLOW"m0 = 0\n");

        return INFINITY_ROOTS;
    }

    /* только с не ноль */
    else if (IsEqual (CoeffEquation->b, 0)) {

        if (solution_need)
            printf (BRIGHT_YELLOW"m%lg != 0\n", CoeffEquation->c);

        return NO_ROOTS;
    }

    /* только а 0 */
    else {

        if (solution_need)
            printf (BRIGHT_YELLOW"mx = %c%lg/%lg\n", (GetOneSign(CoeffEquation->c * CoeffEquation->b, SING_MINUS_WHITESPACE)),
            fabs(CoeffEquation->c), fabs(CoeffEquation->b));

        *x1_ptr = -CoeffEquation->c/CoeffEquation->b;
        return ONE_ROOT;
    }
}

void PrintAnswer (int k_roots, double x1, double x2) {
    // вывод ответа
    switch (k_roots) {
        case NO_ROOTS:
             printf (BRIGHT_GREEN"\nNo roots\n");
             break;

        case ONE_ROOT:
             printf (BRIGHT_GREEN"\nx1 = %lg\n", x1);
             break;

        case TWO_ROOTS:
             printf (BRIGHT_GREEN"\nx1 = %lg, x2 = %lg\n", x1, x2);
             break;

        case INFINITY_ROOTS:
             printf (BRIGHT_GREEN"\nAn infinite number of roots\n\n");
             break;

        default:
             printf (BRIGHT_RED"\nA strange error\n\n");
    }
}

int IsEqual (double a, double b) {
    return fabs (a - b) <= EPSILON;
}

int EnterVariables(CoeffCase* CoeffEquation) {
    // проверяем корректность адресов
    assert (CoeffEquation != NULL);

    int error_flag = NO_ERROR;

    // послание пользователю
    printf (RESET"\nCoefficients of the quadratic equation\n");
    printf (RESET"a, b, c:\n\n");

    /* получаем коэффициенты */
    int number_wrong_coefficient = scanf ("%lg %lg %lg", &CoeffEquation->a, &CoeffEquation->b, &CoeffEquation->c);
    error_flag = (number_wrong_coefficient == 3) ? NO_ERROR : number_wrong_coefficient;

    while (char d = getchar() != '\n') {
        //проверяем, что пользователь потом не напечатал странных символов
        if (!isspace(d) && error_flag != number_wrong_coefficient)
            error_flag = WRONG_SYMBOLS_AFTER;
    }

    if (error_flag != NO_ERROR)
        ErrorNotification (error_flag);

    return error_flag;
}

void ErrorNotification (int error) {
// оповещаем пользователя об ошибке и ее типе
// проверка, что WRONG_FIRST_COEFFICIENT = 0, иначе error не состыкуется с кейсами по значению
    switch (error) {
        case WRONG_FIRST_COEFFICIENT:
            printf (BRIGHT_RED"You have entered incorrect WRONG_FIRST_COEFFICIENT coefficient\n");
            break;

        case WRONG_SECOND_COEFFICIENT:
            printf (BRIGHT_RED"You have entered incorrect WRONG_SECOND_COEFFICIENT coefficient\n");
            break;

        case WRONG_THIRD_COEFFICIENT:
            printf (BRIGHT_RED"You have entered incorrect WRONG_THIRD_COEFFICIENT coefficient\n");
            break;

        case WRONG_SYMBOLS_AFTER:
            printf (BRIGHT_RED"You typed strange symbols after the coefficients. You must have made a typo\n");
            break;

        default:
            printf (BRIGHT_RED"Don't know what to do, hah\n");
    }
}

bool QuestionContinue (void) {
    // высылаем пользователю вопрос на повторение
    printf (RESET"\nDo you want to continue? Print 'Y' (yes) or 'N' (not).\n");

    // считываем, хочет ли он повторения
    char world[3] = "";
    scanf ("%1s", world);
    SkipUnuselessCharacters ();
    world[0] = toupper(world[0]);

    return ((strcmp (world, "Y") == 0) ? true : false);
}

void CheckCorrectResponses (const CoeffCase* CoeffEquation, double x1, double x2, int k_roots) {
    // проверка корректности вычислений вычислений
    switch (k_roots) {
        case NO_ROOTS:
            if (CoeffEquation->b * CoeffEquation->b - 4 * CoeffEquation->a * CoeffEquation->c < 0)
                PrintRight ();

            else
                PrintGaveWrongAnswer ();

            break;

        case INFINITY_ROOTS:
            if (IsEqual (CoeffEquation->a, 0) && IsEqual (CoeffEquation->b, 0) && IsEqual (CoeffEquation->c, 0))
                PrintRight ();

            else
                PrintGaveWrongAnswer ();

            break;

        case ONE_ROOT:
            if (IsEqual (x1 * x1 * CoeffEquation->a + CoeffEquation->b * x1 + CoeffEquation->c, 0))
                PrintRight ();

            else
                PrintGaveWrongAnswer ();

            break;

        case TWO_ROOTS:
                 if (IsEqual (x1 * x1 * CoeffEquation->a + CoeffEquation->b * x1 + CoeffEquation->c, 0) &&
                     IsEqual (x2 * x2 * CoeffEquation->a + CoeffEquation->b * x2 + CoeffEquation->c, 0))
                PrintRight ();

            else
                PrintGaveWrongAnswer ();

            break;

        default:
            printf (BRIGHT_RED"Don't know what to do, hah\n");
    }
}

bool QuestionSolution (void) {
    // высылаем пользователю вопрос на повторение
    printf (RESET"\n\nDo you want to see the solution? Print 'Y' (yes) or 'N' (not).\n");

    // считываем, хочет ли он повторения
    char world[2] = "";
    scanf ("%1s", world);
    SkipUnuselessCharacters ();
    world[0] = toupper(world[0]);

    return ((strcmp (world, "Y") == 0) ? true : false);
}

char GetOneSign (double x, int execution_option) {
    // узнаем и возвращаем нужный знак
    switch (execution_option) {
        case SING_MINUS_WHITESPACE:
            return ((x <= 0) ? ' ': '-');

        case SIGN_PLUS_MINUS:
            return ((x >= 0) ? '+': '-');

        case SING_WHITESPACE_MINUS:
            return ((x >= 0) ? ' ': '-');
    }
}

bool CheckEnum (void) {
    if (NO_ROOTS == 0 && ONE_ROOT == 1 && TWO_ROOTS == 2 && INFINITY_ROOTS == 3 &&
        WRONG_FIRST_COEFFICIENT == 0 && WRONG_SECOND_COEFFICIENT == 1 && WRONG_THIRD_COEFFICIENT == 2 &&
        WRONG_SYMBOLS_AFTER == 3 && NO_ERROR == 4)
        return false;

    printf (BRIGHT_RED"EnumValues aren't associated with a variable\n");
    return true;
}

void RunTests (void) {
    double x1 = 0, x2 = 0;

    const TestCase tests_array[] = {
//      {{a, b, c}, n_roots_ref,    x1_ref,  x2_ref}
        {{0, 0, 0}, INFINITY_ROOTS, NAN,     NAN}, //1
        {{1, 0, 0}, ONE_ROOT,       0,       NAN}, //2
        {{0, 1, 0}, ONE_ROOT,       0,       NAN}, //3
        {{0, 1, 2}, ONE_ROOT,       -2,      NAN}, //4
        {{0, 0, 1}, NO_ROOTS,       NAN,     NAN}, //5
        {{1, 1, 1}, NO_ROOTS,       NAN,     NAN}, //6
        {{1, 2, 1}, ONE_ROOT,       -1,      NAN}, //7
        {{1, 5, 6}, TWO_ROOTS,      -2,      -3 }, //8
        {{2, 1, 3}, NO_ROOTS,       NAN,     NAN}  //9
    };

    int number_test = sizeof(tests_array) / sizeof(TestCase);

    for (int i = 1; i <= number_test; i++) {
        assert (0 <= i - 1 && i - 1 < number_test);
        RunOneTest (tests_array[i - 1], i);
    };

    for (int i = number_test + 1; i < number_test * 2; i++) {
        x1 = rand () / rand ();
        x2 = rand () / rand ();
        double a = rand () / rand ();
        a = (IsEqual (a, 0)) ? 1 : a;
        x1 = (IsEqual (x1, 0) && IsEqual (x2, 0)) ? 1 : x1;
//                                    {{a, b,            c      }, n_roots_ref, x1_ref, x2_ref}
        TestCase test_random_struct = {{a, -a * (x1 + x2), a * x1 * x2}, TWO_ROOTS,   x1,     x2};

        RunOneTest (test_random_struct, i);
    }
}

void RunOneTest (const TestCase Test, int number_test) {

    bool flag_not_failed = true;
    double x1_get = NAN, x2_get = NAN;
    int n_roots = RootsFind (&Test.CoeffEquation, &x1_get, &x2_get, false);

    if (n_roots == Test.n_roots_ref) {
        switch (n_roots) {
            case NO_ROOTS:
            case INFINITY_ROOTS:
                break;

            case ONE_ROOT:
                flag_not_failed  = IsEqual (x1_get, Test.x1_ref);

                break;

            case TWO_ROOTS:
                flag_not_failed = (IsEqual (x1_get, fmax (Test.x1_ref, Test.x2_ref)) &&
                                   IsEqual (x2_get, fmin (Test.x1_ref, Test.x2_ref)));

                break;

            default:
                flag_not_failed = false;
            }
    }

    else
        flag_not_failed = false;

    if (!flag_not_failed)
        printf (BRIGHT_RED"Test %d FAILED:\n"
                " a = %3.3lg, b = %3.3lg, c = %3.3lg\n"
                "Expected: %d roots, x1 = %3.3lg, x2 = %3.3lg\n"
                "Got:      %d roots, x1 = %3.3lg, x2 = %3.3lg\n",
                number_test,
                Test.CoeffEquation.a, Test.CoeffEquation.b, Test.CoeffEquation.c,
                Test.n_roots_ref, Test.x1_ref, Test.x2_ref,
                n_roots,          x1_get,      x2_get);
}

void PrintRight (void) {
    printf (BRIGHT_GREEN"Everything is right!\n");
}

void PrintGaveWrongAnswer (void) {
    printf (BRIGHT_RED"Sorry, I gave you an incorrect answer.(\nLearn to do the calculations yourself.\n");
}

void PracticeSolvingEquations (void) {
    FILE* file_address = fopen("test_verification_data.txt", "r"); // лучше просто указать что за файл
//  проверяем корректность адреса файла
    assert (file_address != NULL);

    do {
        double a = NAN, b = NAN, c = NAN;
        int n_roots_get = -1, n_roots_ref = -1;
        double x1_ref = NAN, x2_ref = NAN;
        double x1_get = NAN, x2_get = NAN;

        fscanf (file_address, "%lg %lg %lg %d %lg %lg", &a, &b, &c, &n_roots_ref, &x1_ref, &x2_ref);
        printf (RESET"\n%lg*x^2 %c %lg*x %c %lg = 0\n", a, GetOneSign (b, SIGN_PLUS_MINUS), fabs(b), GetOneSign (c, SIGN_PLUS_MINUS), fabs(c));
        printf (RESET"Number of roots: ");
        scanf ("%d", &n_roots_get);
        SkipUnuselessCharacters ();

        if (n_roots_ref == n_roots_get){
            switch (n_roots_ref) {
                case NO_ROOTS:
                    UpliftingMoodAfterCorrectAnswers ();
                    break;

                case ONE_ROOT:
                    printf (RESET"x1: ");
                    scanf ("%lg", &x1_get);
                    SkipUnuselessCharacters ();

                    if (IsEqual ((x1_get), (x1_ref)))
                        UpliftingMoodAfterCorrectAnswers ();

                    else {
                        UpliftingMoodAfterWrongAnswers (a, b, c, &x1_get, &x2_get);
                        printf (BRIGHT_GREEN"Right answer:\nx1 = %lg", x1_get);
                    }

                    break;

                case TWO_ROOTS:
                    printf (RESET"x1, x2: ");
                    scanf ("%lg %lg", &x1_get, &x2_get);
                    SkipUnuselessCharacters ();

                    if (IsEqual (fmax (x1_get, x2_get), fmax(x1_ref, x2_ref)) &&
                        IsEqual (fmin (x1_get, x2_get), fmin(x1_ref, x2_ref)))
                        UpliftingMoodAfterCorrectAnswers ();

                    else {
                        UpliftingMoodAfterWrongAnswers (a, b, c, &x1_get, &x2_get);
                        printf (BRIGHT_GREEN"Right answer:\nx1 = %lg, x2 = %lg", x1_get, x2_get);
                    }

                    break;

                default:
                    printf (BRIGHT_RED"You must have a typo\n");
            }
        }

        else {
            UpliftingMoodAfterWrongAnswers (a, b, c, &x1_get, &x2_get);
            printf (BRIGHT_GREEN"Right answer:\nx1 = %lg, x2 = %lg\n", x1_get, x2_get);
        }
    } while (QuestionContinue ());

    fclose (file_address);
}

void UpliftingMoodAfterCorrectAnswers () {
    printf (BRIGHT_GREEN"\nAbsolutely flawless! You nailed roots with pinpoint accuracy.\n"
            "Your grasp formulas (or the discriminant) is crystal clear.\n"
            "Keep up the great work - you're solving like a pro!\n");
}

int QuestionPracticeOrSolution (void) {
    // высылаем пользователю вопрос на повторение
    printf (BRIGHT_MAGENTA"Hello! Great to see you. Would you like to solve a quadratic equation together?\n"
            "Or would you prefer to practice with a set of examples —"
            "I'll give you equations, you send me your answers,"
            "and I'll check them? Choose what suits you best, and let's get started!.\n"
            "Print 'P' (PRACTICE)  or 'S' (SOLUTION)\n");

    // считываем, хочет ли он повторения
    char world[1] = "";
    scanf ("%1s", world);
    SkipUnuselessCharacters ();
    world[0] = toupper(world[0]);

    if (strcmp ((world), "P") == 0)
        return PRACTICE;

    else if (strcmp ((world), "S") == 0)
        return SOLUTION;

    else
        printf (BRIGHT_RED"I don't understand you\n");

    return WRONG_CHOICE;
}

void SolveEquation (CoeffCase* CoeffEquation) {
            do {
            // считываем значения переменных
                if (EnterVariables (CoeffEquation) == NO_ERROR){

                    /* Ищем корни */
                    double x1 = NAN, x2  = NAN;
                    bool solution_need = QuestionSolution ();
                    int k_roots = RootsFind (CoeffEquation, &x1, &x2, solution_need);

                    /* вывод ответа */
                    PrintAnswer (k_roots, x1, x2);

                    // проверка ответов
                    CheckCorrectResponses (CoeffEquation, x1, x2, k_roots);
                }
            } while (QuestionContinue ());
}

void UpliftingMoodAfterWrongAnswers (double a, double b, double c, double* x1_ptr, double* x2_ptr) {
    printf (BRIGHT_RED"\nYou made a mistake.\n"
            "No worries - it's just one equation. Mistakes are clues, not failures.\n"
            "Let's look at the solution!\n");
    CoeffCase CoeffEquation = {.a = a, .b = b, .c = c};
    RootsFind (&CoeffEquation, x1_ptr, x2_ptr, true);
}

void SkipUnuselessCharacters (void){
    while (char d = getchar() != '\n') {
        if (!isspace(d))
            ;
    }
}
