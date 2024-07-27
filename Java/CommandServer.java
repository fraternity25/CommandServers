//CommandServer.java
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.ProcessBuilder.Redirect;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;

public class CommandServer 
{
    public static void main(String[] args) 
    {
        int port = 12345; //80;
        String publicIp = "0.0.0.0"; //"172.29.80.1";
        
        try (ServerSocket serverSocket = new ServerSocket(port, 2, InetAddress.getByName(publicIp)))
        {
            System.out.println("Server is running...");
            
            while (true) 
            {
                Socket clientSocket = serverSocket.accept();
                new ClientHandler(clientSocket).start();
            }
        }
        catch (IOException e) 
        {
            e.printStackTrace();
        }
    }
}

class ClientHandler extends Thread 
{
    private Socket clientSocket;
    private String clientName;
    
    public ClientHandler(Socket socket) 
    {
        this.clientSocket = socket;
    }

    public boolean system(String command, boolean redirectInput, boolean redirectOutput, StringBuilder output) 
    {
        boolean success = true;
        try 
        {
            ProcessBuilder builder = new ProcessBuilder("cmd.exe", "/c", command);
            if (redirectInput) 
            {
                builder.redirectInput(Redirect.INHERIT);
            }
            
            if (redirectOutput) 
            {
                builder.redirectOutput(Redirect.INHERIT);
                builder.redirectError(Redirect.INHERIT);
            }
            Process process = builder.start();
            
            // Capturing stdout
            StringBuilder stdout = new StringBuilder();
            if (output != null) 
            {
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
                String line;
                while ((line = reader.readLine()) != null) 
                {
                    stdout.append(line).append(System.lineSeparator());
                }
            }

            // Capturing stderr
            StringBuilder stderr = new StringBuilder();
            if (output != null) 
            {
                BufferedReader reader = new BufferedReader(new InputStreamReader(process.getErrorStream()));
                String line;
                while ((line = reader.readLine()) != null) 
                {
                    stderr.append(line).append(System.lineSeparator());
                }
            }
            
            process.waitFor();
            success = (process.exitValue() == 0);

            if (output != null) 
            {
                if (stdout.length() > 0) 
                {
                    output.append(stdout);
                }
                if (stderr.length() > 0) 
                {
                    output.append(stderr);
                }
            }
        } 
        catch (Exception e) 
        {
            e.printStackTrace();
            success = false;
        }
        return success;
    }
    
    public void run() 
    {
        try (BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()))) 
        {
            // Read the client's name first
            clientName = in.readLine();
            System.out.println("Client connected: " + clientName);
            
            String input;
            while ((input = in.readLine()) != null) 
            {
                System.out.println("Command from " + clientName + ": " + input);
                if (input.equals("exit")) 
                {
                    break;
                }
                else if(input.equals("cls"))
                {
                    system("cls", false, true, null);
                    continue;
                }
                //System.out.print("output: ");
                system("colorline.exe 1", false, true, null);
                System.out.print("output: ");
                StringBuilder output = new StringBuilder();
                boolean success = system(input, true, false, output);
                if(success)
                {
                    system("colorline.exe a", false, true, null);
                    System.out.println(output);
                    //System.out.println("Command executed successfully!");
                }
                else
                {
                    system("colorline.exe 4", false, true, null);
                    System.out.println(output);
                    //System.out.println("Command execution failed!");
                }
                system("colorline.exe 7", false, true, null);
            }
        }
        catch (IOException e) 
        {
            e.printStackTrace();
        }
        finally 
        {
            try 
            {
                clientSocket.close();
            }
            catch (IOException e) 
            {
                e.printStackTrace();
            }
            System.out.println("Client disconnected: " + clientName);
        }
    }
}
