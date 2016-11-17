

        import java.io.BufferedReader;
        import java.io.IOException;
        import java.io.InputStreamReader;
        import java.io.PrintWriter;
        import java.net.Socket;
        import java.util.Random;

public class Client implements Runnable {
    private Socket socket;
    private String myID;
    @Override
    public void run() {
        System.out.println("Connected to node");

        new Thread(new inputThread(socket)).start();
        try {
            Thread.sleep(500);
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
        BufferedReader consoleReader = new BufferedReader(new InputStreamReader(System.in));
        String id = "";
        String message = "";
        while(true) {
            try {
                System.out.println("Enter another client ID:");
                id = consoleReader.readLine();
                System.out.println("Enter your message:");
		        String tempMessage = "";
                while ((tempMessage = consoleReader.readLine())!= null)
        			if (message == "") 
        				message += tempMessage;
        			else
        				message += "\n" + tempMessage;
		        //System.out.println("\n\n" + message);
                printToServer(socket, id, message + "\n\b");
            } catch (Exception e) {
                System.out.println(e);
            }
        }


    }
    public void printToServer(Socket s,String id, String message){
        try {
            PrintWriter writer = new PrintWriter(s.getOutputStream());
            writer.println(myID+" to "+id+":"+message);
            writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public Client(Socket socket) {
        this.socket = socket;
        try {
            BufferedReader readerB = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            myID = readerB.readLine().substring(9);
            System.out.println(myID);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        try {
            Socket s = new Socket(args[0], 1488);
            new Thread(new Client(s)).start();
        } catch (IOException e) {
            System.out.println("Failed to connect");
            e.printStackTrace();
        }
    }

    class inputThread implements Runnable {
        private BufferedReader reader;

        @Override
        public void run() {
            while (true) {
                try {
                    System.out.println(reader.readLine());
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        public inputThread(Socket socket) {
            try {
                reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

}

