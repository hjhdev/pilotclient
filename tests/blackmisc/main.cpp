#include "testblackmiscmain.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include <QCoreApplication>
#include <QDebug>

using namespace BlackMiscTest;

/*!
 * Starter for test cases
 * \brief main
 * \param argc
 * \param argv
 * \return
 */
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BlackMisc::registerMetadata();

    CBlackMiscTestMain::unitMain(argc, argv);
    Q_UNUSED(a);

    // bye
    return 0;
}
