#include "Chess.h"
#include <QApplication>

///
/// main
/// \param argc
/// \param argv
/// \return
///
int main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  Chess w;
  w.show();

  return a.exec();
}
