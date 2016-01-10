#include "gui/MainWindow.h"

#include "algorithms/Algorithm.h"
#include "algorithms/Lucy.h"
#include "algorithms/LucyOMP.h"
#include "algorithms/Cartoonize.h"
#include "algorithms/CartoonizeOMP.h"
#include "algorithms/Parameters.h"

#include <memory>
#include <QApplication>
#include <QLineEdit>
#include <QLabel>

// Autoren: Yildiz Kasimay, Artjom Poljakow, Yadullah Duman

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    // structure containing parameters you can set
    Parameters params;

    // Create main window
    MainWindow window(params);


    // instantiate algorithms
    //------------------------------------
    // YOU SHOULD ADD SOMETHING HERE..
    //------------------------------------
    std::shared_ptr<Algorithm> lucy(new Lucy);
    std::shared_ptr<Algorithm> lucyomp(new LucyOMP);
    std::shared_ptr<Algorithm> cartoonize(new Cartoonize);
    std::shared_ptr<Algorithm> cartoonizeomp(new CartoonizeOMP);

    // add algorithms to gui
    //------------------------------------
    // YOU SHOULD ADD SOMETHING HERE..
    //------------------------------------
    window.addAlgorithm("Lucy", lucy.get());
    window.addAlgorithm("LucyOMP", lucyomp.get());
    window.addAlgorithm("Cartoonize", cartoonize.get());
    window.addAlgorithm("CartoonizeOMP", cartoonizeomp.get());




    //------------------------------------
    // YOU SHOULD ADD SOMETHING HERE..
    //------------------------------------
    // new parameters

    // Lucy Iterations
    QLineEdit *lineEdit_lucyIterations = window.addIntLineEdit("Lucy Iterations:");
    lineEdit_lucyIterations->setText(QString::number(params.lucyIterations));
    MainWindow::connect(lineEdit_lucyIterations, SIGNAL(textChanged(const QString&)), &params, SLOT(setLucyIterations(QString)));

    // Kernel Size for Cartoonizing
    QLineEdit *lineEdit_kernelSize = window.addDoubleLineEdit("Kernel Size Cartoonizer:");
    lineEdit_kernelSize->setText(QString::number(params.kernelSize));
    MainWindow::connect(lineEdit_kernelSize, SIGNAL(textChanged(const QString&)), &params, SLOT(setKernelSize(QString)));

    // Sigma D for Cartoonizing
    QLineEdit *lineEdit_sigma_d = window.addDoubleLineEdit("Sigma D Cartoonizer:");
    lineEdit_sigma_d->setText(QString::number(params.sigma_d));
    MainWindow::connect(lineEdit_sigma_d, SIGNAL(textChanged(const QString&)), &params, SLOT(setSigma_D(QString)));

    // Sigma R for Cartoonizing
    QLineEdit *lineEdit_sigma_r = window.addDoubleLineEdit("Sigma R Cartoonizer:");
    lineEdit_sigma_r->setText(QString::number(params.sigma_r));
    MainWindow::connect(lineEdit_sigma_r, SIGNAL(textChanged(const QString&)), &params, SLOT(setSigma_R(QString)));

    // Tau for Cartoonizing
    QLineEdit *lineEdit_tau = window.addDoubleLineEdit("Tau Cartoonizer:");
    lineEdit_tau->setText(QString::number(params.tau));
    MainWindow::connect(lineEdit_tau, SIGNAL(textChanged(const QString&)), &params, SLOT(setTau(QString)));



    // finally show the main window
    window.show();

    // Run main event loop
    return app.exec();
}
