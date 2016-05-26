import Cell.*;
import org.omg.CosNaming.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.io.*;

class PhoneCallbackServant extends PhoneCallbackPOA
{
	String myNum;

	PhoneCallbackServant (String num)
	{
		myNum = num;
	};

	public int sendSMS(String fromNum, String message)
	{
		System.out.println(myNum+": new message from "+fromNum+": "+message);
		return (0);
	};

	public void PTTmsg(String message)
	{
		System.out.println("PTT message: "+message);
	};

	public String getNum()
	{
		return (myNum);
	};
};

class ORBThread extends Thread
{
	ORB myOrb;

	ORBThread(ORB orb)
	{
		myOrb = orb;
	};

	//thread launcher
	public void run()
	{
		myOrb.run();
	};
};

public class Phone
{

	public static void main(String args[])
	{
		try
		{
			BufferedReader inpt  = new BufferedReader(new InputStreamReader(System.in));
			System.out.println("Enter your number");
			String myNum = inpt.readLine();
			ORB orb = ORB.init(args, null);

			POA rootPOA = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
			rootPOA.the_POAManager().activate();
			PhoneCallbackServant listener  = new PhoneCallbackServant(myNum);
			rootPOA.activate_object(listener);
			//Servant reference
			PhoneCallback ref = PhoneCallbackHelper.narrow(rootPOA.servant_to_reference(listener));

			//Naming context
			org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
			NamingContext ncRef = NamingContextHelper.narrow(objRef);

			//Base station name to reference conversion
			NameComponent nc = new NameComponent("BaseStation", "");
			NameComponent path[] = {nc};
			Station stationRef = StationHelper.narrow(ncRef.resolve(path));

			//registration
			if(stationRef .register(ref, myNum) == 1)
				System.out.println("registered");
			else
			{
				System.out.println("registration failed");
				return;
			}

			//separate thread for phone orb
			ORBThread orbThr = new ORBThread(orb);
			orbThr.start();

			String msg, toNum;
			System.out.println("Enter number");
			toNum = inpt.readLine();
			PhoneCallback PTTpartner = stationRef.requirePTT(toNum);
			while (true)
			{
				msg = inpt.readLine();
				PTTpartner.PTTmsg(msg);
			}

		} catch (Exception e)
		{
			e.printStackTrace();
		};


	};

};
