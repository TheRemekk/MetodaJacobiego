#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <mpreal.h>
#include <Interval.h>
#include <string>
#include <regex>
#include <QScrollBar>

// ZMIENNE GLOBALNE KONIECZNE W ZASTOSOWANIU PROGRAMU
int n;
int mit = 0;
int currentIndexI = 1;
int currentIndexJ = 1;
int currentIndexB = 1;
int currentIndexX = 1;
bool isData = false;

std::regex number_arithmeticRegex(R"(([-+]?(?:0|[1-9]\d*)(?:\.\d+)?,[-+]?(?:0|[1-9]\d*)(?:\.\d+)?))");
std::regex numberRegex(R"(([-+]?(?:0|[1-9]\d*)(?:\.\d+)?))");
std::regex integerRegex(R"((-?(?!0\d+)\d+))");
std::regex exp_nRegex("^\\d+\\.?\\d*e[-+]?\\d+$");

std::string A_str[100][100];
std::string B_str[100];
std::string x1_str[100];
std::string epsilon;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // USTAWIENIA STANDARDOWE W APLIKACJI
    setFixedSize(800, 580);

    QString styleSheet = "background-color: rgb(173, 216, 230);";
    this->setStyleSheet(styleSheet);

    styleSheet = "background-color: white;";
    ui->nTxtBox->setStyleSheet(styleSheet);
    ui->dataTxtBox->setStyleSheet(styleSheet);
    ui->resultTxtBox->setStyleSheet(styleSheet);

    styleSheet = "color: red;";
    ui->errorLbl->setStyleSheet(styleSheet);
    ui->error2Lbl->setStyleSheet(styleSheet);
    ui->error3Lbl->setStyleSheet(styleSheet);

    QPushButton *button = ui->calcBtn;
    styleSheet = "background-color: gray;";
    button->setStyleSheet(styleSheet);

    styleSheet = "QScrollBar:vertical {"
                 "    background-color: gray;"
                 "}"
                 "QScrollBar::handle:vertical {"
                 "    background-color: gray;"
                 "}";
    ui->resultTxtBox->verticalScrollBar()->setStyleSheet(styleSheet);

    ui->errorLbl->hide();
    ui->error3Lbl->hide();
    ui->dataLbl->hide();
    ui->dataTxtBox->hide();
    ui->rBtn1->setChecked(true);
    ui->resultTxtBox->setReadOnly(true);

    // PODAWANIE WARTOŚCI N ODPOWIADAJĄCEJ ZA WIELKOŚĆ MACIERZY
    connect(ui->nTxtBox, &QTextEdit::textChanged, this, [&]() {
        if (ui->nTxtBox->toPlainText().endsWith("\n")) {
            QString value = ui->nTxtBox->toPlainText().trimmed();
            std::string value_str = value.toStdString();
            bool iscorrect_n = true;

            if (std::regex_match(value_str, integerRegex)) {
                int number = std::stoi(value_str);
                if (number > 1 && number < 100) {
                    n = number;
                    ui->nLbl->hide();
                    ui->nTxtBox->hide();
                    QString labelName = QString("A[%1][%2] =").arg(currentIndexI).arg(currentIndexJ);
                    ui->dataLbl->setText(labelName);
                    ui->dataLbl->show();
                    ui->dataTxtBox->show();
                    ui->dataTxtBox->setFocus();
                    if (ui->errorLbl->isVisible()) ui->errorLbl->hide();
                    // rBtn1 -> Arytmetyka zwykła
                    ui->rBtn1->setEnabled(false);
                    // rBtn2 -> Arytmetyka zmiennopozycyjna
                    ui->rBtn2->setEnabled(false);
                    // rBtn3 -> Arytmetyka przedziałowa zmiennopozycyjna
                    ui->rBtn3->setEnabled(false);
                } else iscorrect_n = false;
            } else iscorrect_n = false;

            if (iscorrect_n == false) ui->errorLbl->show();
        }
    });

    // PODAWANIE WARTOŚCI TABLIC A ORAZ B DO MACIERZY WRAZ Z ILOŚCIĄ ITERACJI
    connect(ui->dataTxtBox, &QTextEdit::textChanged, this, [=]() {
        if (ui->dataTxtBox->toPlainText().endsWith("\n")) {
            QString value = ui->dataTxtBox->toPlainText().trimmed();
            QString labelName;
            QString errorType = "0";

            int i = currentIndexI-1;
            int j = currentIndexJ-1;
            int k = currentIndexB-1;
            int x = currentIndexX-1;
            bool iscorrect = true;

            std::string intervalBounds = value.toStdString();

            if (currentIndexX <= n) {
                if (std::regex_match(intervalBounds, number_arithmeticRegex) && ui->rBtn3->isChecked()) {
                    std::size_t commaPos = intervalBounds.find(',');
                    std::string a_str, b_str;
                    a_str = intervalBounds.substr(0, commaPos);
                    b_str = intervalBounds.substr(commaPos + 1);
                    long double a = std::stold(a_str);
                    long double b = std::stold(b_str);
                    if ((i == j) && (a == 0 || b == 0)) errorType = "2";
                }
                else if (std::regex_match(intervalBounds, numberRegex)) {
                    long double num = std::stold(intervalBounds);
                    if (i == j && num == 0) errorType = "2";
                }
                else errorType = "1";
            }
            else {
                if (mit == 0) {
                    if (std::regex_match(intervalBounds, integerRegex)) {
                        int number = std::stoi(intervalBounds);
                        if (number > 0) {
                            mit = number;
                        } else errorType = "1";
                    } else errorType = "1";
                } else {
                    if (std::regex_match(intervalBounds, exp_nRegex)) {
                        epsilon = intervalBounds;
                        isData = true;
                        ui->error2Lbl->hide();
                        ui->dataLbl->hide();
                        ui->dataTxtBox->hide();
                    } else errorType = "1";
                }
            }

            if(errorType == "1") {
                iscorrect = false;
                if (!ui->errorLbl->isVisible()) ui->errorLbl->show();
                if (ui->error3Lbl->isVisible()) ui->error3Lbl->hide();
            }
            else if(errorType == "2") {
                iscorrect = false;
                if (ui->errorLbl->isVisible()) ui->errorLbl->hide();
                if (!ui->error3Lbl->isVisible()) ui->error3Lbl->show();
                //if (currentIndexB <= n) ui->error3Lbl->show();
            }
            else {
                if (ui->errorLbl->isVisible()) ui->errorLbl->hide();
                if (ui->error3Lbl->isVisible()) ui->error3Lbl->hide();
            }

            if (iscorrect) {
                if (currentIndexI <= n && currentIndexJ <= n) { // Tutaj powinno wystąpić A[][]
                    A_str[i][j] = intervalBounds;
                }
                else if (currentIndexB <= n) { // Tutaj powinno wystąpić B[]
                    B_str[k] = intervalBounds;
                    currentIndexB++;
                }
                else if (currentIndexX <= n){
                    x1_str[x] = intervalBounds;
                    currentIndexX++;
                }

                if (currentIndexJ < n) {
                    currentIndexJ++;
                }
                else if (currentIndexJ == n) {
                    currentIndexI++;
                    currentIndexJ = 1;
                }

                if (currentIndexI <= n && currentIndexJ <= n) { // Tutaj powinno wystąpić A[][]
                    labelName = QString("A[%1][%2] =").arg(currentIndexI).arg(currentIndexJ);
                } else if (currentIndexB <= n) { // Tutaj powinno wystąpić B[]
                    labelName = QString("b[%1] =").arg(currentIndexB);
                } else if (currentIndexX <= n) { // Tutaj powinno wystąpić B[]
                    labelName = QString("x[%1] =").arg(currentIndexX);
                }
                else { // Tutaj powinno wystąpić ilość iteracji
                    if (mit == 0) labelName = "Maksymalna l. iteracji =";
                    else labelName = "Wartość epsilon =";
                }

                //ui->resultTxtBox->setText(value);
                ui->dataLbl->setText(labelName);
                ui->dataTxtBox->clear();
                ui->dataTxtBox->setFocus();
            }
        }
    });

    // OPEROWANIE PRZYCISKIEM OBLICZANIA (WYBIERANIE FUNKCJI Z RADIOBUTTONA, RESETOWANIE WARTOŚCI TABLIC)

    connect(ui->calcBtn, &QPushButton::clicked, this, &MainWindow::on_calcBtn_clicked);
    connect(this, &MainWindow::calculationsFinished, this, &MainWindow::resetCalculations);
}
void MainWindow::on_calcBtn_clicked()
{
    if (isData == true) {
        if (ui->rBtn1->isChecked()) {
            Jacobi();
        } else if (ui->rBtn2->isChecked()) {
            Jacobi_fl_point();
        } else {
            Jacobi_interval();
        }
    }
}

void MainWindow::Jacobi() // ARYTMETYKA ZWYKŁA
{
    long double A[n][n];
    long double B[n];
    long double N[n]; // N = D^(-1)
    long double M[n][n]; // M = -D^(-1)*(L+U)
    long double x1[n];
    long double x2[n];
    // x(n+1) = M*x + N*B
    long double eps = std::stold(epsilon);
    long double A_check[n][2 * n];

    int it = mit;
    int st = 0;
    bool is_eps_limit = false;

    if (n < 1) st = 1;

    // Podawanie wartości macierzy A(x), b oraz wartości początkowych x1
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = std::stold(A_str[i][j]);
        }
        B[i] = std::stold(B_str[i]);
    }
    for (int i = 0; i < n; i++) {
        x1[i] = std::stold(x1_str[i]);
    }

    // Sprawdzanie osobliwości macierzy A
    if (st == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A_check[i][j] = A[i][j];
            }
            for (int j = n; j < 2 * n; j++) {
                A_check[i][j] = (j - n == i) ? 1.0 : 0.0;
            }
        }

        for (int i = 0; i < n; i++) {
            if (A_check[i][i] == 0.0) {
                st = 2;
                break;
            }
            for (int j = i + 1; j < n; j++) {
                long double ratio = A_check[j][i] / A_check[i][i];
                for (int k = i; k < 2 * n; k++) {
                    A_check[j][k] -= ratio * A_check[i][k];
                }
            }
        }
        if (A_check[n - 1][n - 1] == 0.0) st = 2;
    }

    // Obliczanie wyników dla x, biorąc pod uwagę max. liczbę iteracji (mit) oraz wartość epsilon (eps) i wyznaczając ilość iteracji (it), jeśli uzyskamy pożądaną dokładność
    for (int i = 0; i < n; i++)
        N[i] = 1.0 / A[i][i];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j)
                M[i][j] = 0.0;
            else
                M[i][j] = -(A[i][j] * N[i]);
        }
    }

    for (int k = 0; k < mit; k++) {
        for (int i = 0; i < n; i++) {
            x2[i] = N[i] * B[i];
            for (int j = 0; j < n; j++) {
                x2[i] += M[i][j] * x1[j];
            }
        }
        bool stop = true;
        for (int i = 0; i < n; i++) {
            if (std::abs(x1[i] - x2[i]) > eps) {
                stop = false;
                break;
            }
        }
        if (stop) {
            it = k+1;
            is_eps_limit = true;
            break;
        }
        for (int i = 0; i < n; i++) {
            x1[i] = x2[i];
        }
    }
    if (st == 0 && !is_eps_limit) st = 3;

    // Wypisywanie wyników
    std::ostringstream oss;
    oss << std::scientific << std::uppercase << std::setprecision(14);

    for (int i = 0; i < n; i++) {
        oss << "x[" << i + 1 << "] = " << x1[i] << std::endl;
    }

    oss << "it = " << it << std::endl;
    oss << "st = " << st << std::endl;
    std::string result = oss.str();
    ui->resultTxtBox->setText(QString::fromStdString(result));
    emit calculationsFinished();
}

void MainWindow::Jacobi_fl_point() // ARYTMETYKA ZMIENNOPOZYCYJNA (NA MPFR C++, Z PRECYZJĄ 100)
{
    mpfr::mpreal A[n][n];
    mpfr::mpreal B[n];
    mpfr::mpreal N[n]; // N = D^(-1)
    mpfr::mpreal M[n][n]; // M = -D^(-1)*(L+U)
    mpfr::mpreal x1[n];
    mpfr::mpreal x2[n];
    // x(n+1) = M*x + N*B
    mpfr::mpreal eps = mpfr::mpreal(epsilon);
    mpfr::mpreal A_check[n][2 * n];

    mpfr::mpreal::set_default_prec(100);

    std::string var_in_str, a_str, b_str;
    std::ostringstream oss;
    bool is_eps_limit = false;
    int it = mit;
    int st = 0;

    if (n < 1) st = 1;

    // Podawanie wartości macierzy A(x), b oraz wartości początkowych x1
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            var_in_str = A_str[i][j];
            A[i][j] = mpfr::mpreal(var_in_str);
        }
        var_in_str = B_str[i];
        B[i] = mpfr::mpreal(var_in_str);
    }
    for (int i = 0; i < n; i++) {
        var_in_str = x1_str[i];
        x1[i] = mpfr::mpreal(var_in_str);
    }

    // Sprawdzanie osobliwości macierzy A
    if (st == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A_check[i][j] = A[i][j];
            }
            for (int j = n; j < 2 * n; j++) {
                A_check[i][j] = (j - n == i) ? mpfr::mpreal(1.0) : mpfr::mpreal(0.0);
            }
        }

        for (int i = 0; i < n; i++) {
            if (A_check[i][i] == mpfr::mpreal(0.0)) {
                st = 2;
                break;
            }
            for (int j = i + 1; j < n; j++) {
                mpfr::mpreal ratio = A_check[j][i] / A_check[i][i];
                for (int k = i; k < 2 * n; k++) {
                    A_check[j][k] -= ratio * A_check[i][k];
                }
            }
        }
        if (A_check[n - 1][n - 1] == mpfr::mpreal(0.0)) st = 2;
    }

    // Obliczanie wyników dla x, biorąc pod uwagę max. liczbę iteracji (mit) oraz wartość epsilon (eps) i wyznaczając ilość iteracji (it), jeśli uzyskamy pożądaną dokładność
    for (int i = 0; i < n; i++)
        N[i] = mpfr::mpreal(1.0) / A[i][i];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j)
                M[i][j] = mpfr::mpreal(0.0);
            else
                M[i][j] = -(A[i][j] * N[i]);
        }
    }

    for (int k = 0; k < mit; k++) {
        for (int i = 0; i < n; i++) {
            x2[i] = N[i] * B[i];
            for (int j = 0; j < n; j++) {
                x2[i] += M[i][j] * x1[j];
            }
        }
        bool stop = true;
        for (int i = 0; i < n; i++) {
            if (mpfr::abs(x1[i] - x2[i]) > eps) {
                stop = false;
                break;
            }
        }
        if (stop) {
            it = k+1;
            is_eps_limit = true;
            break;
        }
        for (int i = 0; i < n; i++) {
            x1[i] = x2[i];
        }
    }
    if (st == 0 && !is_eps_limit) st = 3;

    // Wypisywanie wyników do QTextEdit
    oss << std::scientific << std::uppercase << std::setprecision(14);

    for (int i = 0; i < n; i++) {
        oss << "x[" << i + 1 << "] = " << x1[i] << std::endl;
    }

    oss << "it = " << it << endl;
    oss << "st = " << st << endl;
    std::string result = oss.str();
    ui->resultTxtBox->setText(QString::fromStdString(result));
    emit calculationsFinished();
}

void MainWindow::Jacobi_interval() // ARYTMETYKA PRZEDZIAŁOWA ZMIENNOPOZYCYJNA
{
    interval_arithmetic::Interval<long double> A[n][n];
    interval_arithmetic::Interval<long double> B[n];
    interval_arithmetic::Interval<long double> N[n]; // N = D^(-1)
    interval_arithmetic::Interval<long double> M[n][n]; // M = -D^(-1)*(L+U)
    interval_arithmetic::Interval<long double> x1[n];
    interval_arithmetic::Interval<long double> x2[n];
    // x(n+1) = M*x + N*B
    interval_arithmetic::Interval<long double> eps = interval_arithmetic::IntRead<long double>(epsilon);
    interval_arithmetic::Interval<long double> A_check[n][2 * n];

    std::string var_in_str, a_str, b_str;
    std::size_t commaPos;
    std::ostringstream oss;
    bool is_eps_limit = false;
    int it = mit;
    int st = 0;

    if (n < 1) st = 1;

    // Podawanie wartości macierzy A(x), b oraz wartości początkowych x1
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            var_in_str = A_str[i][j];
            commaPos = var_in_str.find(',');
            a_str = var_in_str.substr(0, commaPos);
            b_str = var_in_str.substr(commaPos + 1);
            A[i][j].a = interval_arithmetic::LeftRead<long double>(a_str);
            A[i][j].b = interval_arithmetic::RightRead<long double>(b_str);
        }
        var_in_str = B_str[i];
        commaPos = var_in_str.find(',');
        a_str = var_in_str.substr(0, commaPos);
        b_str = var_in_str.substr(commaPos + 1);
        B[i].a = interval_arithmetic::LeftRead<long double>(a_str);
        B[i].b = interval_arithmetic::RightRead<long double>(b_str);
    }
    for (int i = 0; i < n; i++) {
        var_in_str = x1_str[i];
        commaPos = var_in_str.find(',');
        a_str = var_in_str.substr(0, commaPos);
        b_str = var_in_str.substr(commaPos + 1);
        x1[i].a = interval_arithmetic::LeftRead<long double>(a_str);
        x1[i].b = interval_arithmetic::RightRead<long double>(b_str);
    }

    // Sprawdzanie osobliwości macierzy A
    if (st == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                A_check[i][j] = A[i][j];
            }
            for (int j = n; j < 2 * n; j++) {
                A_check[i][j] = (j - n == i) ? interval_arithmetic::IntRead<long double>("1.0") : interval_arithmetic::IntRead<long double>("0.0");
            }
        }

        for (int i = 0; i < n; i++) {
            if (A_check[i][i].a == 0.0 && A_check[i][i].b == 0.0) {
                st = 2;
                break;
            }
            for (int j = i + 1; j < n; j++) {
                interval_arithmetic::Interval<long double> ratio = interval_arithmetic::IDiv(A_check[j][i], A_check[i][i]);
                for (int k = i; k < 2 * n; k++) {
                    A_check[j][k] = interval_arithmetic::ISub(A_check[j][k], interval_arithmetic::IMul(ratio, A_check[i][k]));
                }
            }
        }
        if (A_check[n - 1][n - 1].a == 0.0 && A_check[n - 1][n - 1].b == 0.0) st = 2;
    }

    // Obliczanie wyników dla x, biorąc pod uwagę max. liczbę iteracji (mit) oraz wartość epsilon (eps) i wyznaczając ilość iteracji (it), jeśli uzyskamy pożądaną dokładność
    for (int i = 0; i < n; i++) {
        N[i] = interval_arithmetic::IDiv(interval_arithmetic::IntRead<long double>("1.0"), A[i][i]);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                M[i][j].a = interval_arithmetic::LeftRead<long double>("0.0");
                M[i][j].b = interval_arithmetic::RightRead<long double>("0.0");
            }
            else {
                M[i][j] = (-1) * interval_arithmetic::IMul(A[i][j], N[i]);
            }
        }
    }
    for (int k = 0; k < mit; k++) {
        for (int i = 0; i < n; i++) {
            x2[i] = interval_arithmetic::IMul(N[i], B[i]);
            for (int j = 0; j < n; j++) {
                x2[i] = interval_arithmetic::IAdd(x2[i], interval_arithmetic::IMul(M[i][j], x1[j]));
            }
        }
        bool stop = true;
        for (int i = 0; i < n; i++) {
            interval_arithmetic::Interval<long double> diff = interval_arithmetic::IAbs(x2[i] - x1[i]);
            if (diff.a > eps.a || diff.b > eps.a) {
                stop = false;
                break;
            }
        }
        if (stop) {
            it = k+1;
            is_eps_limit = true;
            break;
        }
        for (int i = 0; i < n; i++) {
            x1[i] = x2[i];
        }
    }
    if (st == 0 && !is_eps_limit) st = 3;

    // Wypisywanie wyników do QTextEdit
    oss << std::scientific << std::uppercase << std::setprecision(14);

    for (int i = 0; i < n; i++) {
        oss << "x[" << i + 1 << "] = [" << x1[i].a << ", " <<  x1[i].b << "]" << endl;
        oss << "szerokość = " << x1[i].GetWidth() << endl;
    }

    oss << "it = " << it << endl;
    oss << "st = " << st << endl;
    std::string result = oss.str();
    ui->resultTxtBox->setText(QString::fromStdString(result));
    emit calculationsFinished();
}

void MainWindow::resetCalculations() // RESETOWANIE TABLIC I ZMIENNYCH, WIDOCZNOŚCI W DESIGNIE OKIENKOWYM
{
    ui->nLbl->show();
    ui->nTxtBox->show();
    ui->dataLbl->hide();
    ui->dataTxtBox->hide();
    ui->rBtn1->setEnabled(true);
    ui->rBtn2->setEnabled(true);
    ui->rBtn3->setEnabled(true);
    ui->nTxtBox->setText("");
    ui->dataTxtBox->setText("");
    ui->error2Lbl->show();
    currentIndexI = 1;
    currentIndexJ = 1;
    currentIndexB = 1;
    currentIndexX = 1;
    mit = 0;
    epsilon = "";
    isData = false;
}
MainWindow::~MainWindow()
{
    delete ui;
}
