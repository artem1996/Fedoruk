

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.Random;


public class MainServer implements Runnable {
    ServerSocket serverSocket;

    static HashMap<Integer, Socket> serverMap = new HashMap<>(); // serverId and Socket. TUBES?????
    static HashMap<String, String> clientMap = new HashMap<>();//clientID and serverID

    public static void main(String[] args) {
        new Thread(new MainServer(1337)).start();
    }

    @Override
    public void run() {
        try {
            while (true) {
                Socket connection = serverSocket.accept();
                System.out.println("connection established");
                new Thread(new inputListener(connection)).start();
                Thread.sleep(500);

            }
        } catch (Exception e) {
            System.out.println("Connection establishing error");
        }
    }

    public MainServer(int socketNum) {
        try {
            serverSocket = new ServerSocket(socketNum);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static class inputListener implements Runnable {
        BufferedReader reader;
        int clientID;
        PrintWriter writer;

        @Override
        public void run() {
            try {
                while (true) {
                    String line = reader.readLine();
                    System.out.println(line);
                    if (line.startsWith("CONC:")){
                        clientMap.put(line.substring(5),String.valueOf(clientID));
                        System.out.println("adding "+ line.substring(5) + " and "+String.valueOf(clientID));
                    }
                    else if (clientMap.containsKey(line.split(" to ")[1].split(":")[0])){
                        String a1 = clientMap.get(line.split(" to ")[1].split(":")[0]);
                        System.out.println(a1);// get serverID
                        System.out.println(serverMap.containsKey(Integer.parseInt(a1)));
                        PrintWriter w = new PrintWriter(serverMap.get(Integer.parseInt(a1)).getOutputStream());
                        w.println(line);
                        w.flush();
                    }

                }
            }
            catch (Exception e){
                System.out.println("Input listener error");
            }
        }
        public void broadcastMessage(String message, Socket s){
            System.out.println("this is broadcastMessage");
            System.out.println("message is "+message);
            try {
                PrintWriter w = new PrintWriter(s.getOutputStream());
                w.println(message);
                w.flush();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }


        public inputListener(Socket socket) {
            try {
                Random r = new Random();
                clientID = r.nextInt(1000);
                reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                writer = new PrintWriter(socket.getOutputStream());


                writer.println("Your ID: " + clientID);
                writer.flush();
                System.out.println("Thread " + clientID + " created");
                serverMap.put(clientID, socket); //add server to map
            } catch (IOException e) {
                e.printStackTrace();
            }

        }
    }
}
