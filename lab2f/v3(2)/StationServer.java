import Cell.*;
import org.omg.CosNaming.*;
import org.omg.CosNaming.NamingContextPackage.*;
import org.omg.CORBA.*;
import org.omg.PortableServer.*;
import org.omg.PortableServer.POA;

import java.util.Properties;

import java.util.HashMap;

class StationServant extends StationPOA
{
	HashMap m_hm_registered;
	
	public StationServant()
	{
		m_hm_registered = new HashMap();
	}

	public int register (PhoneCallback objRef, String phoneNum)
	{
		if(!m_hm_registered.containsKey(phoneNum))
		{
			m_hm_registered.put(phoneNum, objRef);
			System.out.println("Station: phone number "+phoneNum+" is registered");
		
		}
		else
		{
			System.out.println("Station: ERROR! phone number "+phoneNum+" is already registered");
			return(0);
		}
		return (1);
	};

	public int sendSMS (String fromNum, String toNum, String message)
	{
		if(m_hm_registered.containsKey(toNum))
		{
			PhoneCallback phoneRef = (PhoneCallback)m_hm_registered.get(toNum);
			System.out.println("Station: message from "+fromNum+" to "+toNum);
			phoneRef.sendSMS(fromNum, message);
		}
		else
		{
			System.out.println("Station: message to unregistered number "+toNum);
			return(0);
		}
		return (1);
	};
	
	public PhoneCallback requirePTT(String toNum)
	{
		if(m_hm_registered.containsKey(toNum))
		{
			PhoneCallback objRef = (PhoneCallback)m_hm_registered.get(toNum);
			return(objRef);
		}
		else
		{
			System.out.println("Station: PTT to unregistered number "+toNum);
			return(null);
		}
	};
};

public class StationServer
{

	public static void main(String args[])
	{
		try
		{
			ORB orb = ORB.init(args, null);

			POA rootpoa = POAHelper.narrow(orb.resolve_initial_references("RootPOA"));
			rootpoa.the_POAManager().activate();

			StationServant servant = new StationServant();

			org.omg.CORBA.Object ref = rootpoa.servant_to_reference(servant);
			Station sref = StationHelper.narrow(ref);

			org.omg.CORBA.Object objRef = orb.resolve_initial_references("NameService");
			NamingContextExt ncRef = NamingContextExtHelper.narrow(objRef);

			String name = "BaseStation";
			NameComponent path[] = ncRef.to_name( name );
			ncRef.rebind(path, sref);

			System.out.println("Station ready...");

			orb.run();
		} 
		catch (Exception e)
		{
			System.err.println("ERROR: " + e);
			e.printStackTrace(System.out);
		};
	};
};
