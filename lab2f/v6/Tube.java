import Cell.*;
import org.omg.CosNaming.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.io.*;

class TubeCallbackServant extends TubeCallbackPOA {
	String myNum;

	TubeCallbackServant (String num) {
		myNum = num;
	};

	public int sendSMS(String fromNum, String message) {
		String br = "";
		try {
			br = new String (message.getBytes("ISO-8859-1"),"UTF-8" );
		} catch(UnsupportedEncodingException uee) {
			uee.printStackTrace();
		}
		System.out.println(myNum+": new message from "+fromNum+": "+br);
		return (0);
	};

	public String getNum() {
		return (myNum);
	};
};

class ORBThread extends Thread {
	ORB myOrb;

	ORBThread(ORB orb) {
		myOrb = orb;
	};

	public void run() {
		myOrb.run();
	};
};

public class Tube {

	public static void main(String args[]) {
		boolean isPPT = false;
		try {
			String myNum = "123";
			ORB orb = ORB.init(args, null);

			POA rootPOA = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
			rootPOA.the_POAManager().activate();
			TubeCallbackServant listener  = new TubeCallbackServant(myNum);
			rootPOA.activate_object(listener);
			TubeCallback ref = TubeCallbackHelper.narrow(rootPOA.servant_to_reference(listener));

			org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
			NamingContext ncRef = NamingContextHelper.narrow(objRef);

			NameComponent nc = new NameComponent("BaseStation", "");
			NameComponent path[] = {nc};
			Station stationRef = StationHelper.narrow(ncRef.resolve(path));

			stationRef .register(ref, myNum);
			System.out.println("The tube was registered on the base station");

			ORBThread orbThr = new ORBThread(orb);
			orbThr.start();

			BufferedReader inpt  = new BufferedReader(new InputStreamReader(System.in));
			String msg;
			TubeCallback tc = ref;
			while (true)
			{
				msg = inpt.readLine();
				String delims = "[\\[\\]]+";
				String[] token = msg.split(delims);
				if (token.length < 3 )
				{
					System.out.println("ïÛÉÂËÁ ÆÏÒÍÁÔÁ ÓÏÏÂÝÅÎÉÑ. îÅÏÂÈÏÄÉÍÏ: [ÎÏÍÅÒ ÁÂÏÎÅÎÔÁ] ÓÏÏÂÝÅÎÉÅ");
				}
				else
				{
					msg = token[2].trim();
					if (msg.contains("ppt"))
					{
						tc = stationRef.getOponent(token[1]);
						isPPT = true;
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
			}

		} 
		catch (Exception e)
		{
			e.printStackTrace();
		};


	};

};

