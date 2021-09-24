
import java.io.IOException;
import java.util.ArrayList;
import java.io.File;
import java.io.FileInputStream;
import java.lang.*;

public class CengTreeParser 
{	
	public static ArrayList<CengGift> parseGiftsFromFile(String filename)
	{
		ArrayList<CengGift> giftList = new ArrayList<CengGift>();
				
		// You need to parse the input file in order to use GUI tables.
		// TODO: Parse the input file, and convert them into CengGifts
		
		File file = new File(filename);
		FileInputStream fis = null;
		
		try{
			fis = new FileInputStream(file);
			
			int c;
			int giftKey = 0;
			StringBuilder giftName = new StringBuilder();
			StringBuilder giftMaterial = new StringBuilder();
			StringBuilder giftColor = new StringBuilder();
			
			
			while(true){
				for(c = fis.read(); c != '|'; c = fis.read()){
					giftKey = 10 * giftKey + c - '0';
				}

				for(c = fis.read(); c != '|'; c = fis.read()){
					giftName.append((char) c);
				}
				
				for(c = fis.read(); c != '|'; c = fis.read()){
					giftMaterial.append((char) c);
				}
				
				for(c = fis.read(); c != '\n'; c = fis.read()){
					giftColor.append((char) c);
				}
				
				CengGift g = new CengGift(giftKey, giftName.toString(), giftMaterial.toString(), giftColor.toString());
				giftList.add(g);
				
				giftKey = 0;
				giftName.setLength(0);
				giftMaterial.setLength(0);
				giftColor.setLength(0);
				
				
				c = fis.read();
				if(c == - 1) break;
				else giftKey = 10 * giftKey + c - '0';
				
				



			}
			
			fis.close();
		}catch(IOException e){
			e.printStackTrace();
		}
		
		return giftList;
	}
	

	
	public static void startParsingCommandLine() throws IOException
	{
		
		
		// TODO: Start listening and parsing command line -System.in-.
		// There are 4 commands:
		// 1) quit : End the app, gracefully. Print nothing, call nothing, just break off your command line loop.
		// 2) add : Parse and create the gift, and call CengChristmasList.addGift(newlyCreatedGift).
		// 3) search : Parse the key, and call CengChristmasList.searchGift(parsedKey).
		// 4) print : Print the whole tree, call CengChristmasList.printTree().

		// Commands (quit, add, search, print) are case-insensitive.

		
		int c;
		
		try{
			while(true){
				StringBuilder command = new StringBuilder();
				for(c = System.in.read(); c != '\n' && c != '|' && c!=-1; c = System.in.read()){
					if(!Character.isWhitespace(c))
						command.append((char) c);
				}
				String commandString = command.toString();
				commandString.toLowerCase();

				
				if(commandString.equals("quit")){
					break;
				}

				else if(commandString.equals("print")){
					CengChristmasList.printTree();
				}
				else if(commandString.equals("search")){
					int giftKey = 0;
					for(c = System.in.read(); c!= '\n' && c!=-1; c = System.in.read()){
						if(!Character.isWhitespace(c))
							giftKey = 10 * giftKey + c - '0';
					}
					CengChristmasList.searchGift(giftKey);
				}
				else if(commandString.equals("add")){
					
					int giftKey = 0;
					StringBuilder giftName = new StringBuilder();
					StringBuilder giftMaterial = new StringBuilder();
					StringBuilder giftColor = new StringBuilder();
					
					for(c = System.in.read(); c != '|'; c = System.in.read()){
						giftKey = 10 * giftKey + c - '0';
					}

					for(c = System.in.read(); c != '|'; c = System.in.read()){
						giftName.append((char) c);
					}
					
					for(c = System.in.read(); c != '|'; c = System.in.read()){
						giftMaterial.append((char) c);
					}
					
					for(c = System.in.read(); c != '\n' && c!=-1 ; c = System.in.read()){
							giftColor.append((char) c);
					}
					if(Character.isWhitespace(giftColor.charAt(giftColor.length() -1)))
						giftColor.setLength(giftColor.length() - 1);
					
					CengGift g = new CengGift(giftKey, giftName.toString(), giftMaterial.toString(), giftColor.toString());
					CengChristmasList.addGift(g);
					
				}
			}
			
		}catch(IOException e){
			e.printStackTrace();
		}
		

	}
}
