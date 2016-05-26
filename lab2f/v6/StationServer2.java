import Cell.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;
import java.io.*;

import java.util.Properties;
import java.util.Iterator;
import java.util.Map;
import java.util.HashMap;

class StationServant extends StationPOA {
	Map<String, TubeCallback> mp = new HashMap<String, TubeCallback>();
	Station stationRef;
	String stationName;

	public int register (TubeCallback objRef, String phoneNum) {
		mp.put(phoneNum, objRef);
		System.out.println("Station: Tube was registered "+phoneNum);
		return (1);
	};

	public int registerStation(Station objRef, String phoneNum)
	{
		stationRef = objRef;
		stationName = phoneNum;
		System.out.println("Station " + phoneNum + "was connected");
		return (1);
	}
	
	public TubeCallback getOponent(String num)
	{
		//if (mp.containsKey(num))
		{
			return mp.get(num);
		}
	}

	public int sendSMS (String fromNum, String toNum, String message) {
		System.out.println("Station: Tube "+fromNum+" is sending message to "+toNum);
		if (mp.containsKey(toNum))
		{
			mp.get(toNum).sendSMS(fromNum, message);
		}
		else
		{
			System.out.println("Tube unregistered. Sending message to Server StationServer");
			stationRef.sendSMS(fromNum, toNum, message);
			/*String mes = "Unregistered tube " + toNum + ".";
			try 
			{
				mes = new String(mes.getBytes("UTF-8"), "ISO-8859-1");
			} 
			catch(UnsupportedEncodingException uee) 
			{
				uee.printStackTrace();
			}
			System.out.println("Call to unregistered tube " + toNum + ".");
			mp.get(fromNum).sendSMS("SERVER", mes);*/
		}
		return (1);
	};
};

public class StationServer2 {

	public static void main(String args[]) {
		try{
			ORB orb = ORB.init(args, null);

			POA rootpoa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
			rootpoa.the_POAManager().activate();

			StationServant servant = new StationServant();

			org.omg.CORBA.Object ref = rootpoa.servant_to_reference(servant);
			Station sref = StationHelper.narrow(ref);

			org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
			NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);

			String name = "BaseStation2";
			NameComponent path[] = ncRef.to_name( name );
			ncRef.rebind(path, sref);

			System.out.println("Server ready and waiting...");

			orb.run();
		} 
		catch (Exception e) {
			System.err.println("Error: " + e);
			e.printStackTrace(System.out);
		};
	};
};
