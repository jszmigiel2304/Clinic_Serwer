###################################################
# Clinic_Serwer (https://github.com/jszmigiel2304/Clinic_Serwer)

Utworzona w Qt/C++, w modelu MVC aplikacja serwerowa, kt�ra przetwarza, ��dania otrzymane z aplikacji Clinic_Client ( https://github.com/jszmigiel2304/Clinic_Client ) i w zale�no�ci od typu ��dnia, pobiera odpowiednie dane z dw�ch baz danych (Authorization lub Clinic), lub te� sama przetwarza dane i przekazuje je z powrotem do Clinic_Client.

Wa�ne jest �e aplikacja Clinic_Client nie posiada �adnych informacji, gdzie i jak przechowywane s� dane. Przez u�ytkownika [authenticator], kt�ry ma uprawnienia tylko do odczytu danych w [Authorization.users], nast�puje zalogowanie, a nast�pnie ju�, przy pomocy do��czonych do ka�dego ��dania [user_name] i [user_password], wysy�ane jest do serwera ��danie.
################################################
G�owne klasy tej aplikacji to:
------------------------------------------------
c_ClinicTcpServer: dziedziczy z QTcpServer, konfiguruje, uruchamia, i przetwarza przychodz�ce po��cznia
------------------------------------------------
c_ClientConnection: obiekty tej klasy, s� tworzone w c_ClinicTcpServer::incomingConnection(), dla ka�dego przychodz�cego po��czenia i to za ich pomoc� odbywa si� komunikacja Client <-> Serwer
-------------------------------------------------
c_MySqlDatabaseController: tu przechowywane s� informacje, z jakimi bazami danych komunikuje si� Clinic_Serwer, 

Dane potrzebne do po��cznia z bazami danych przechowywane s� w pliku [config.ini] i tam mog� by� zmienione, lub te� przez okno ustawie� w samej aplikacji

funkcje c_MySqlDatabaseController::exe(QString query, QString destDatabase, QList< QMap<QString,QVariant> > * results, QStringList * errors) oraz ju� dla spersonalizowanych ��da� c_MySqlDatabaseController::exe(QString query, QString destDatabase, QString name, QString password, QList<QMap<QString, QVariant> > *results, QStringList *errors), "odpytuj�" tutaj podane [destDatabase] bazy danych
-------------------------------------------------
c_actionExecutive: w tej klasie przetwarzane s� wszystkie �adania i przygotowywane odpowiedzi
################################################
Struktura wysy�anych pakiet�w znajduje si� w pliku:
serializations.xlsx
################################################