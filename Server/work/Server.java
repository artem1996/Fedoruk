
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.Random;


public class Server implements Runnable {
    private ServerSocket s;
    static String myID;
    static private Socket mainSocket; // socket to MainServer
    static HashMap<Integer, Socket> tubeMap = new HashMap<>();

    public Server(int socketNum) {
        try {
            mainSocket = new Socket("127.0.0.1", 1337); //connect to mainServer
            s = new ServerSocket(socketNum); // open ServerSocket
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void run() {
        try {
            new Thread(new MainServerListener()).start();
            while (!Thread.interrupted()) {
                Socket connection = s.accept();
                new Thread(new ServerClientThread(connection)).start();
                System.out.println("connection established");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    public static class MainServerListener implements Runnable {
        @Override
        public void run() {
            BufferedReader reader = null;
            try {
                reader = new BufferedReader(new InputStreamReader(mainSocket.getInputStream()));

                myID = reader.readLine().substring(9);
                System.out.println("This server ID: "+myID);
                while (true) {// ????? DEBUG ME PLS
                    String msg = reader.readLine();
                    System.out.println("Received from mainServer:\n" + msg);
                    int targetID = Integer.parseInt(msg.split(" to ")[1].split(":")[0]);
                    if (tubeMap.containsKey(targetID))
                        ServerClientThread.sendMessage(msg.split(" to ")[0] + ": " + msg.split(":")[1], tubeMap.get(targetID));
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

    }

    public static class ServerClientThread implements Runnable {
        int threadId;
        PrintWriter writer;
        BufferedReader reader;

        public ServerClientThread(Socket connection) {
            try {
                Random r = new Random();
                writer = new PrintWriter(connection.getOutputStream(), true);
                reader = new BufferedReader(new InputStreamReader(connection.getInputStream()));
                threadId = r.nextInt(10000);
                writer.println("Your ID: " + threadId);
                sendMessage("CONC:"+threadId, mainSocket);
                writer.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
            System.out.println("Thread " + threadId + " created");
            tubeMap.put(threadId, connection); //add tube to map

        }

        @Override
        public void run() { //listen to tube
            try {
                String line = "";
                while (true) {
        			String tempLine = "";
        			while (!(tempLine = reader.readLine()).isEmpty() && tempLine.charAt(0)!='\b') {
        				line += "\n" + tempLine;
                        //sleep(500);
                        //System.err.println(line);
                    }
                            
                    System.err.println(line);

                    int targetID = Integer.parseInt(line.split(" to ")[1].split(":")[0]);
                    if (tubeMap.containsKey(targetID)) {
                        sendMessage(line.split(" to ")[0] + ": " + line.split(":")[1], tubeMap.get(targetID));
                    } else {
                        sendMessage(line, mainSocket);
                    }
                    line = "";
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        public static void sendMessage(String message, Socket outputSocket) {
            try {
                PrintWriter w = new PrintWriter(outputSocket.getOutputStream());
                w.println(message);
                w.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }


    public static void main(String[] args) {
        new Thread(new Server(1488)).start();
        System.out.println("server started");

    }
}
