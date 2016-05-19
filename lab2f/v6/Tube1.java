import Cell.*;
import org.omg.CosNaming.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.io.*;

// Класс вызова телефонной трубки
class TubeCallbackServant extends TubeCallbackPOA {
	String myNum;	// Номер трубки

	// Конструктор класса
	TubeCallbackServant (String num) {
		myNum = num;
	};

	// Метод обработки принятого сообщения
	public int sendSMS(String fromNum, String message) {
		String br = "";
		try {
			br = new String (message.getBytes("ISO-8859-1"),"UTF-8" );
		} catch(UnsupportedEncodingException uee) {
			uee.printStackTrace();
		}
		System.out.println(myNum+": принято сообщение от "+fromNum+": "+br);
		return (0);
	};

	// Метод, возвращающий номер трубки
	public String getNum() {
		return (myNum);
	};
};

// Класс, используемый для создания потока управления
class ORBThread extends Thread {
	ORB myOrb;

	// Конструктор класса
	ORBThread(ORB orb) {
		myOrb = orb;
	};

	// Метод запуска потока
	public void run() {
		myOrb.run();
	};
};

// Класс, имитирующий телефонную трубку
public class Tube1 {

	public static void main(String args[]) {
		boolean isPPT = false;
		try {
			String myNum = "456";	// Номер трубки
			// Создание и инициализация ORB
			ORB orb = ORB.init(args, null);

			//Создание серванта для IDL-интерфейса TubeCallback
			POA rootPOA = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
			rootPOA.the_POAManager().activate();
			TubeCallbackServant listener  = new TubeCallbackServant(myNum);
			rootPOA.activate_object(listener);
			// Получение ссылки на сервант
			TubeCallback ref = TubeCallbackHelper.narrow(rootPOA.servant_to_reference(listener));

			// Получение контекста именования
			org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
			NamingContext ncRef = NamingContextHelper.narrow(objRef);

			// Преобразование имени базовой станции в объектную ссылку
			NameComponent nc = new NameComponent("BaseStation", "");
			NameComponent path[] = {nc};
			Station stationRef = StationHelper.narrow(ncRef.resolve(path));

			// Регистрация трубки в базовой станции
			stationRef .register(ref, myNum);
			System.out.println("The tube was registered on the base station");

			// Запуск ORB в отдельном потоке управления
			// для прослушивания вызовов трубки
			ORBThread orbThr = new ORBThread(orb);
			orbThr.start();

			// Бесконечный цикл чтения строк с клавиатуры и отсылки их
			// базовой станции
			BufferedReader inpt  = new BufferedReader(new InputStreamReader(System.in));
			String msg;
			TubeCallback tc = ref;
			while (true)
			{
				msg = inpt.readLine();
				String delims = "[\\[\\]]+";
				String[] token = msg.split(delims);
				if (token.length < 3)
				{
					System.out.println("Ошибка формата сообщения. Необходимо: [номер абонента] сообщение");
				}
				else
				{
					msg = token[2].trim();
					if (msg.contains("ppt"))
					{
						tc = stationRef.getOponent(token[1]);
						isPPT = true;
						//isPptRegim = true;
						//isSetRegim = true;
					}
					else
					{
						try 
						{
							msg = new String(msg.getBytes("UTF-8"), "ISO-8859-1");
						} 
						catch(UnsupportedEncodingException uee)
						{
							uee.printStackTrace();
						}
						if (isPPT)
						{
							tc.sendSMS(myNum, msg);
						}
						else
						{
							stationRef.sendSMS(myNum, token[1], msg);
						}

					}
				}
				// Обратите внимание: номер получателя 7890 в описанной ранее
				// реализации базовой станции роли не играет
			}

		} 
		catch (Exception e)
		{
			e.printStackTrace();
		};


	};

};

