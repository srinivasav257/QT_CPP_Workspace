#include <QTest>
#include <QObject>

class SimpleQtTest : public QObject
{
    Q_OBJECT

private slots:
    void toUpper();
};

void SimpleQtTest::toUpper()
{
    QString str = "Hello";
    QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(SimpleQtTest)
#include "tst_qtest_main.moc"
