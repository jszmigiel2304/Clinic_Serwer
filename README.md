###################################################
# Clinic_Serwer (https://github.com/jszmigiel2304/Clinic_Serwer)

Utworzona w Qt/C++, w modelu MVC aplikacja serwerowa, która przetwarza, ¿¹dania otrzymane z aplikacji Clinic_Client ( https://github.com/jszmigiel2304/Clinic_Client ) i w zale¿noœci od typu ¿¹dnia, pobiera odpowiednie dane z dwóch baz danych (Authorization lub Clinic), lub te¿ sama przetwarza dane i przekazuje je z powrotem do Clinic_Client.

Wa¿ne jest ¿e aplikacja Clinic_Client nie posiada ¿adnych informacji, gdzie i jak przechowywane s¹ dane. Przez u¿ytkownika [authenticator], który ma uprawnienia tylko do odczytu danych w [Authorization.users], nastêpuje zalogowanie, a nastêpnie ju¿, przy pomocy do³¹czonych do ka¿dego ¿¹dania [user_name] i [user_password], wysy³ane jest do serwera ¿¹danie.
################################################
G³owne klasy tej aplikacji to:
------------------------------------------------
c_ClinicTcpServer: dziedziczy z QTcpServer, konfiguruje, uruchamia, i przetwarza przychodz¹ce po³¹cznia
------------------------------------------------
c_ClientConnection: obiekty tej klasy, s¹ tworzone w c_ClinicTcpServer::incomingConnection(), dla ka¿dego przychodz¹cego po³¹czenia i to za ich pomocê odbywa siê komunikacja Client <-> Serwer
-------------------------------------------------
c_MySqlDatabaseController: tu przechowywane s¹ informacje, z jakimi bazami danych komunikuje siê Clinic_Serwer, 

Dane potrzebne do po³¹cznia z bazami danych przechowywane s¹ w pliku [config.ini] i tam mog¹ byæ zmienione, lub te¿ przez okno ustawieñ w samej aplikacji

funkcje c_MySqlDatabaseController::exe(QString query, QString destDatabase, QList< QMap<QString,QVariant> > * results, QStringList * errors) oraz ju¿ dla spersonalizowanych ¿¹dañ c_MySqlDatabaseController::exe(QString query, QString destDatabase, QString name, QString password, QList<QMap<QString, QVariant> > *results, QStringList *errors), "odpytuj¹" tutaj podane [destDatabase] bazy danych
-------------------------------------------------
c_actionExecutive: w tej klasie przetwarzane s¹ wszystkie ¿adania i przygotowywane odpowiedzi
################################################
Struktura wysy³anych pakietów znajduje siê w pliku:
serializations.xlsx
################################################