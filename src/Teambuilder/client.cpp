#include "client.h"
#include "mainwindow.h"
#include "challenge.h"
#include "../Utilities/otherwidgets.h"

#include "../PokemonInfo/pokemonstructs.h"

Client::Client(TrainerTeam *t, const QString &url) : myteam(t), myrelay()
{
    mychallenge = NULL;

    setFixedSize(800, 600);

    QGridLayout *layout = new QGridLayout(this);

    layout->addWidget(myplayers = new QListWidget(), 0, 0, 3, 1);
    layout->addWidget(mychat = new QTextEdit(), 0, 1, 1, 2);
    layout->addWidget(myline = new QLineEdit(), 1, 1, 1, 2);
    layout->addWidget(myexit = new QPushButton(tr("&Exit")), 2, 1);
    layout->addWidget(mysender = new QPushButton(tr("&Send")), 2, 2);

    myplayers->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    myplayers->setContextMenuPolicy(Qt::CustomContextMenu);
    myplayers->setSortingEnabled(true);
    mychat->setReadOnly(true);

    connect(myplayers, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showContextMenu(QPoint)));
    connect(myexit, SIGNAL(clicked()), SIGNAL(done()));
    connect(myline, SIGNAL(returnPressed()), SLOT(sendText()));
    connect(mysender, SIGNAL(clicked()), SLOT(sendText()));

    initRelay();

    relay().connectTo(url, 5080);
}

void Client::showContextMenu(const QPoint &requested)
{
    QIdListWidgetItem *item = dynamic_cast<QIdListWidgetItem*>(myplayers->itemAt(requested));

    if (item)
    {
	QMenu *menu = new QMenu(this);

	QAction *viewinfo = menu->addAction("See &Info", &mymapper, SLOT(map()));
	mymapper.setMapping(viewinfo, item->id());

	connect(&mymapper, SIGNAL(mapped(int)), SLOT(seeInfo(int)));

	menu->exec(mapToGlobal(requested));
    }
}

void Client::sendText()
{
    relay().sendMessage(myline->text());
    myline->clear();
}

QMenuBar * Client::createMenuBar(MainWindow *w)
{
    (void) w;
    return NULL;
}

void Client::initRelay()
{
    connect(&relay(), SIGNAL(connectionError(int, QString)), SLOT(errorFromNetwork(int, QString)));
    connect(&relay(), SIGNAL(protocolError(int, QString)), SLOT(errorFromNetwork(int, QString)));
    connect(&relay(), SIGNAL(connected()), SLOT(connected()));
    connect(&relay(), SIGNAL(disconnected()), SLOT(disconnected()));
    connect(&relay(), SIGNAL(messageReceived(QString)), SLOT(messageReceived(QString)));
    connect(&relay(), SIGNAL(playerReceived(Player)), SLOT(playerReceived(Player)));
    connect(&relay(), SIGNAL(playerLogin(Player)), SLOT(playerLogin(Player)));
    connect(&relay(), SIGNAL(playerLogout(int)), SLOT(playerLogout(int)));
}

void Client::messageReceived(const QString &mess)
{
    printLine(mess);
}

bool Client::playerExist(int id) const
{
    return myplayersinfo.contains(id);
}

Player Client::player(int id) const
{
    Player ret = {id, info(id)};
    return ret;
}

BasicInfo Client::info(int id) const
{
    return myplayersinfo[id];
}

void Client::seeInfo(int id)
{
    if (playerExist(id))
    {
	if (mychallenge != NULL) {
	    mychallenge->raise();
	    mychallenge->activateWindow();
	} else {
	    mychallenge = new ChallengeWindow(player(id));
	    connect(mychallenge, SIGNAL(challenge(int)), SLOT(sendChallenge(int)));
	    connect(mychallenge, SIGNAL(destroyed()), SLOT(clearChallenge()));
	    connect(this, SIGNAL(destroyed()),mychallenge, SLOT(close()));
	}
    }
}

void Client::sendChallenge(int id)
{
    relay().sendChallenge(id);
}

void Client::clearChallenge()
{
    mychallenge = NULL;
}

void Client::errorFromNetwork(int errnum, const QString &errorDesc)
{
    QMessageBox::critical(this, tr("Error while connected to server"), tr("Received error n°%1: %2").arg(errnum).arg(errorDesc));
}

void Client::connected()
{
    printLine(tr("Connected to Server!"));

    relay().login(*team());
}

void Client::disconnected()
{
    printLine(tr("Disconnected from Server!"));
}

TrainerTeam* Client::team()
{
    return myteam;
}

Analyzer &Client::relay()
{
    return myrelay;
}

QTextEdit *Client::mainChat()
{
    return mychat;
}

void Client::playerLogin(const Player& p)
{
    playerReceived(p);
    printLine(tr("%1 logged in.").arg(p.team.name));
}

void Client::playerLogout(int id)
{
    QString name = myplayersinfo[id].name;

    printLine(tr("%1 logged out.").arg(name));

    /* removes the item in the playerlist */
    delete myplayers->takeItem(myplayers->row(myplayersitems[id]));

    myplayersitems.remove(id);
    myplayersinfo.remove(id);
}

void Client::playerReceived(const Player &p)
{
    myplayersinfo.insert(p.id, p.team);

    QIdListWidgetItem *item = new QIdListWidgetItem(p.id, p.team.name);

    myplayersitems.insert(p.id, item);

    myplayers->addItem(item);
}

void Client::printLine(const QString &line)
{
    mainChat()->insertPlainText(line + "\n");
}

QDataStream & operator >> (QDataStream &in, Player &p)
{
    in >> p.id;
    in >> p.team;

    return in;
}

QDataStream & operator << (QDataStream &out, const Player &p)
{
    out << p.id;
    out << p.team;

    return out;
}
