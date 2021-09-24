package ceng.ceng351.stackoverflowdb;

import ceng.ceng351.stackoverflowdb.QueryResult.NameUsernameDateRatingResult;
import ceng.ceng351.stackoverflowdb.QueryResult.UseridUsernameLastlogindateResult;
import ceng.ceng351.stackoverflowdb.QueryResult.UseridUsernameRegistrationdateWeeklyreputationResult;
import ceng.ceng351.stackoverflowdb.QueryResult.UsernameDateRatingResult;
import ceng.ceng351.stackoverflowdb.QueryResult.UsernameMessageRatingAlltimereputationResult;
import ceng.ceng351.stackoverflowdb.QueryResult.UsernameMessageRatingResult;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Types;
import java.util.ArrayList;
import  java.sql.PreparedStatement;


public class STACKOVERFLOWDB implements ISTACKOVERFLOWDB {
	
	private static String user = "e2036218";  
    private static String password = "187773";
    private static String host = "144.122.71.165";
    private static String database = "db2036218";
    private static int port = 3306;
    
   

     
    private Connection con;

	@Override
	public void initialize() {
		// TODO Auto-generated method stub
		
		String url = "jdbc:mysql://" + host + ":" + port + "/" + database;
        
        try {
            Class.forName("com.mysql.jdbc.Driver");
            con =  DriverManager.getConnection(url, user, password);
            
            
        } catch (SQLException e) {
            e.printStackTrace();
        } catch (ClassNotFoundException e) {
            e.printStackTrace();
        }

	}

	@Override
	public int createTables() {
		
		
		int numberofTablesInserted = 0;


		String queryCreateuserTable = "CREATE TABLE IF NOT EXISTS user ( " + 
									   "userID CHAR(10), " + 
									   "username CHAR(30), " + 
									   "registrationDate DATE, " +
									   "lastLoginDate DATE, " +
									   "PRIMARY KEY(userID)"
									   + ");";

		String queryCreatearticleTable = "CREATE TABLE IF NOT EXISTS article (" + 
				   "articleID CHAR(10), " + 
				   "userID CHAR(10), " + 
				   "name CHAR(80), " +
				   "description CHAR(130), " +
				   "date  DATE, " +
				   "rating INT, " +
				   "PRIMARY KEY (articleID), " +
				   "FOREIGN KEY (userID) REFERENCES user(userID) ON DELETE CASCADE);";		
		

		String queryCreatecommentTable = "CREATE TABLE IF NOT EXISTS comment ( " + 
				   "commentID CHAR(10), " + 
				   "articleID CHAR(10), " + 
				   "userID CHAR(10), " +
				   "message CHAR(130), " +
				   "date  DATE, " +
				   "rating INT, " +
				   "PRIMARY KEY (commentID), " +
				   "FOREIGN KEY (articleID)  REFERENCES article(articleID) ON DELETE CASCADE, " +
				   "FOREIGN KEY (userID) REFERENCES user(userID) ON DELETE CASCADE); ";
		
		String queryCreatereputationTable = "CREATE TABLE IF NOT EXISTS reputation ( " + 
				   "reputationID CHAR(10), " + 
				   "userID CHAR(10), " + 
				   "weeklyReputation INT, " +
				   "monthlyReputation INT, " +
				   "yearlyReputation  INT, " +
				   "alltimeReputation INT, " +
				   "PRIMARY KEY (reputationID), " +
				   "FOREIGN KEY (userID)  REFERENCES user(userID) ON DELETE CASCADE); ";
		

		

		try {
			//USER Table
			Statement statement = con.createStatement();
			statement.executeUpdate(queryCreateuserTable);
			numberofTablesInserted++;
			//close
			statement.close();
		}catch (SQLException e) {
			e.printStackTrace();
		}

		try{
			
			//ARTICLE TABLE
			Statement statement = con.createStatement();
			statement.executeUpdate(queryCreatearticleTable);
			numberofTablesInserted++;
			//close
			statement.close();
		}catch (SQLException e) {
			e.printStackTrace();
		}
			
		try{
			//COMMENT TABLE
			Statement statement = con.createStatement();
			statement.executeUpdate(queryCreatecommentTable);
			numberofTablesInserted++;
			//close
			statement.close();
		}catch (SQLException e) {
			e.printStackTrace();
		}
			
		try{
			//REPUTATION
			Statement statement = con.createStatement();
			statement.executeUpdate(queryCreatereputationTable);
			numberofTablesInserted++;
			//close
			statement.close();
			} catch (SQLException e) {
			e.printStackTrace();
		}	
		return numberofTablesInserted;
	}

	@Override
	public int dropTables() {

		int numberofTablesDropped = 0;
		
		String dropuser = "drop table if exists user";
		String article = "drop table if exists article";
		String comment  = "drop table if exists comment";
		String reputation = "drop table if exists reputation";

			
		try {
			Statement statement = this.con.createStatement();
			statement.executeUpdate(reputation);
			numberofTablesDropped++;	
			//close
			statement.close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		
		try{
			Statement statement = this.con.createStatement();
			statement.executeUpdate(comment);
			numberofTablesDropped++;
			//close
			statement.close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		
		try{
			Statement statement = this.con.createStatement();
			statement.executeUpdate(article);
			numberofTablesDropped++;
			//close
			statement.close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		
		try{
			Statement statement = this.con.createStatement();
			//Team Player
			statement.executeUpdate(dropuser);
			numberofTablesDropped++;
			//close
			statement.close();
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		
		return numberofTablesDropped;
	}

	@Override
	public int insertUser(User[] users) {
		// TODO Auto-generated method stub
		
		int numberOfUsersInserted = 0;
		
		try {
			for(User user : users){
				
				String userid = user.getuserID();
				String username = user.getusername();
				String regdate = user.getregistrationDate();
				String lastlogin = user.getlastLoginDate();
				
	
				String query = "INSERT INTO user(userID, username, registrationDate, lastLoginDate) VALUES (?,?,?,?)";
				PreparedStatement ps = con.prepareStatement(query);
				
				ps.setString(1,  userid );
				
				if(username.isEmpty())
					ps.setNull(2, Types.CHAR);
				else
					ps.setString(2, username );
				
				if(regdate.isEmpty())
					ps.setNull(3, Types.DATE);
				else
					ps.setString(3,  regdate  );
				
				if(lastlogin.isEmpty())
					ps.setNull(4, Types.DATE);
				else
					ps.setString(4, lastlogin);
				

				ps.executeUpdate();
				numberOfUsersInserted++;
				
				//close
				ps.close();
				
			}
			
			
			
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		return numberOfUsersInserted;

	}

	@Override
	public int insertArticle(Article[] articles) {
		int numberOfArticlesInserted = 0;
		
		try {

			for(Article article : articles){
				String articleID = article.getarticleID();
				String userID = article.getuserID();
				String name = article.getname();
				String description= article.getdescription();
				String date = article.getdate();
				int rating = article.getrating();
				

	
				String query = "INSERT INTO article(articleID, userID, name, description, date, rating) VALUES (?,?,?,?,?,?)";
				PreparedStatement ps = con.prepareStatement(query);
				
				ps.setString(1,  articleID );
				
				if(userID.isEmpty())
					ps.setNull(2, Types.CHAR);
				else
					ps.setString(2, userID );
				
				if(name.isEmpty())
					ps.setNull(3, Types.CHAR);
				else
					ps.setString(3, name.replace("\'", "\'\'"));
				
				if(description.isEmpty())
					ps.setNull(4, Types.CHAR);
				else
					ps.setString(4, description.replace("\'", "\'\'"));
				
				if(date.isEmpty())
					ps.setNull(5, Types.DATE);
				else
					ps.setString(5, date);
				
				ps.setInt(6, rating);
				

				ps.executeUpdate();
				numberOfArticlesInserted++;
				
				//close
				ps.close();
			}

			
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		return numberOfArticlesInserted;
	}

	@Override
	public int insertComment(Comment[] comments) {
		int numberOfCommentsInserted = 0;
		
		try {

			for(Comment comment: comments){
				

				String commentID = comment.getcommentID();
				String articleID = comment.getarticleID();
				String userID = comment.getuserID();
				String message = comment.getmessage();
				String date = comment.getdate();
				int rating = comment.getrating();
				
	
				String query = "INSERT INTO comment(commentID, articleID, userID, message, date,rating) VALUES (?,?,?,?,?, ?)";
				PreparedStatement ps = con.prepareStatement(query);
				
				ps.setString(1,  commentID);
				
				if(articleID.isEmpty())
					ps.setNull(2, Types.CHAR);
				else
					ps.setString(2, articleID);
				
				if(userID.isEmpty())
					ps.setNull(3, Types.CHAR);
				else
					ps.setString(3,  userID  );
				
				if(message.isEmpty())
					ps.setNull(4, Types.CHAR);
				else
					ps.setString(4, message.replace("\'", "\'\'"));
				
				if(date.isEmpty())
					ps.setNull(5,  Types.DATE);
				else
					ps.setString(5, date);
				
				ps.setInt(6, rating);

				ps.executeUpdate();
				numberOfCommentsInserted++;
				
				//close
				ps.close();
			}

			
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		return numberOfCommentsInserted;
		
	}

	@Override
	public int insertReputation(Reputation[] reputations) {
		int numberOfReputationsInserted = 0;
		
		try {
			for(Reputation reputation: reputations){

				
				
				String repID = reputation.getreputationID();
				String userID = reputation.getuserID();
				int weekly = reputation.getweeklyReputation();
				int monthly = reputation.getmonthlyReputation();
				int alltime = reputation.getalltimeReputation();
				
				

	
				String query = "INSERT INTO reputation(reputationID, userID, weeklyReputation, monthlyReputation, alltimeReputation) VALUES (?,?,?,?,?)";
				PreparedStatement ps = con.prepareStatement(query);
				
				ps.setString(1,  repID );
				
				if(userID.isEmpty())
					ps.setNull(2, Types.CHAR);
				else
					ps.setString(2, userID );
				
				ps.setInt(3, weekly);
				ps.setInt(4, monthly);
				ps.setInt(5, alltime);
				

				ps.executeUpdate();
				numberOfReputationsInserted++;
				
				//close
				ps.close();
			}

			
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		return numberOfReputationsInserted;
	}

	@Override
	public UsernameDateRatingResult[] getArticleHighestRating() {
		
		ArrayList<UsernameDateRatingResult> array = new ArrayList<UsernameDateRatingResult>();
		try{
			String query = "SELECT DISTINCT U.username, A1.date, A1.rating" 
					 + " FROM article A1, user U"
					 + " WHERE A1.userID = U.userID AND A1.rating = (SELECT MAX(A2.rating)"
					 + " FROM article A2)"
					 + " ORDER BY U.username ASC";
		
			Statement statement = con.createStatement();
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				String uname = rs.getString(1);
				String date = rs.getString(2);
				int rating = rs.getInt(3);
				UsernameDateRatingResult x = new UsernameDateRatingResult(uname,date,rating);
				array.add(x);
				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new UsernameDateRatingResult[array.size()]);
	}

	@Override
	public UsernameMessageRatingAlltimereputationResult[] getCommentLowestRating() {
		ArrayList<UsernameMessageRatingAlltimereputationResult> array = new ArrayList<UsernameMessageRatingAlltimereputationResult>();
		try{
			
			//fixle
			String query = " SELECT  DISTINCT U.username, C1.message, C1.rating, R.alltimeReputation"
					+" FROM  user U, comment C1, reputation R"
					+" WHERE C1.userID = U.userID AND"
					      +" U.userID = R.userID AND"
					      +" C1.message NOT LIKE '%mysql%' AND"
					      +" C1.rating = ("
					+" SELECT MIN(C.rating)"
					+" FROM comment C"
					+" WHERE C.message NOT LIKE '%mysql%')"
					+" ORDER BY U.username ASC";
		
			Statement statement = con.createStatement();
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				String uname = rs.getString(1);
				String message= rs.getString(2);
				int rating = rs.getInt(3);
				int alltimerep = rs.getInt(4);
				UsernameMessageRatingAlltimereputationResult x = new UsernameMessageRatingAlltimereputationResult(uname,message,rating, alltimerep);
				array.add(x);
				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new UsernameMessageRatingAlltimereputationResult[array.size()]);
	}

	@Override
	public UseridUsernameRegistrationdateWeeklyreputationResult[] getUseridUsernameAfterGivenDate(
			String Date) {
		
		ArrayList<UseridUsernameRegistrationdateWeeklyreputationResult> array = new ArrayList<UseridUsernameRegistrationdateWeeklyreputationResult>();
		try{
			
			//fixle
			String query = 
					" SELECT DISTINCT U.userID, U.username, U.registrationDate, R.weeklyReputation"
					+ " FROM user U, reputation R"
					+ " WHERE U.registrationDate >"
					+ " \'" + Date +"\'"
					+ " AND U.userID = R.userID"
					+ " ORDER BY U.userID ASC";
		
			Statement statement = con.createStatement();
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				String uID = rs.getString(1);
				String uname= rs.getString(2);
				String regdate = rs.getString(3);
				int weeklyrep = rs.getInt(4);
				UseridUsernameRegistrationdateWeeklyreputationResult x = new UseridUsernameRegistrationdateWeeklyreputationResult(uID,uname,regdate, weeklyrep);
				array.add(x);
				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new UseridUsernameRegistrationdateWeeklyreputationResult[array.size()]);
	}
	

	@Override
	public UsernameMessageRatingResult[] getUsernameMessageRatingMoreThanGivenRating(
			String message, int rating) {
		ArrayList<UsernameMessageRatingResult> array = new ArrayList<UsernameMessageRatingResult>();
		try{
			
			//fixle
			String query = 
					" SELECT DISTINCT U.username, C.message, C.rating "
					+ " FROM comment C, user U "
					+ " WHERE C.message NOT LIKE " +  "\'%" + message.replace("\'", "\'\'") + "%\'"
					+    " AND C.rating > " + rating +
					    " AND C.userID = U.userID "
					 + " ORDER BY U.username ASC";


			Statement statement = con.createStatement();
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				String uname= rs.getString(1);
				String mes = rs.getString(2);
				int rat = rs.getInt(3);
				UsernameMessageRatingResult x = new UsernameMessageRatingResult(uname,mes, rat);
				array.add(x);
				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new UsernameMessageRatingResult[array.size()]);
	}

	@Override
	public int MultiplyComment(String date) {
		// TODO Auto-generated method stub
		int result = 0;
		
		try {
			Statement statement = con.createStatement();

				String queryInsertReputation = "UPDATE comment"
		+ " SET rating = 2 * rating"
		+ " WHERE date >" + "\'" +  date + "\';";
				
				result = statement.executeUpdate(queryInsertReputation);
				
			
			//close
			statement.close();
			
		} catch (SQLException e) {
			e.printStackTrace();
		}
		
		return result;
		
	}

	@Override
	public UseridUsernameLastlogindateResult[] getUsernameMessageRatingCommentedByGivenUser(
			String userID) {
		ArrayList<UseridUsernameLastlogindateResult> array = new ArrayList<UseridUsernameLastlogindateResult>();
		try{
			
			//fixle
			String query = 
					"SELECT DISTINCT U.userID, U.username, U.lastLoginDate "
					+ " FROM user U "
					+ " WHERE U.userID <> " + "\'" + userID + "\'" + " AND NOT EXISTS( "
							  + " SELECT C.commentID "
							  + " FROM comment C "
							  + " WHERE C.userID = " + "\'" + userID + "\' " +  " AND C.articleID NOT IN(" 
									    	    	+ " SELECT C2.articleID"
									    	  	+ " FROM comment C2"
											+ " WHERE U.userID = C2.userID))"
											+ "ORDER BY U.userID";
										


			Statement statement = con.createStatement();
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				String uID= rs.getString(1);
				String uname = rs.getString(2);
				String lastlogin = rs.getString(3);
				UseridUsernameLastlogindateResult x = new UseridUsernameLastlogindateResult(uID, uname, lastlogin);
				array.add(x);
				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new UseridUsernameLastlogindateResult[array.size()]);
	}

	@Override
	public UsernameMessageRatingResult[] getNameUsernameDateRatingMoreThanGivenArticle(
			int rating, String articleID) {
		// TODO Auto-generated method stub

		ArrayList<UsernameMessageRatingResult> array = new ArrayList<UsernameMessageRatingResult>();
		try{
			
			//fixle
			String query = 
					"SELECT DISTINCT U.username, C.message, C.rating"
					+" FROM user U, comment C"
					+" WHERE C.userID = U.userID"
					+" AND C.commentID IN"
					+" (SELECT C.commentID"
					+" FROM article A, comment C"
					+" WHERE A.articleID = C.articleID"
					+    " AND A.articleID =" + "\'" + articleID + "\'"
					+    " AND C.rating >" + rating
					+" )"
					+" ORDER BY U.username ASC";


			Statement statement = con.createStatement();
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				String uname= rs.getString(1);
				String mes = rs.getString(2);
				int rat = rs.getInt(3);
				UsernameMessageRatingResult x = new UsernameMessageRatingResult(uname, mes, rat);
				array.add(x);
				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new UsernameMessageRatingResult[array.size()]);
	}

	@Override
	public NameUsernameDateRatingResult[] getUsernameDateRatingNotCommentedByAnyUser() {
		// TODO Auto-generated method stub
		

		ArrayList<NameUsernameDateRatingResult> array = new ArrayList<NameUsernameDateRatingResult>();
		try{
			
			//fixle
			String query = 
					"SELECT DISTINCT A.name, U.username, A.date, A.rating"
					+" FROM article A, comment C, user U"
					+" WHERE A.articleID = C.articleID"
					+"    AND U.userID = C.userID"
					+" GROUP BY A.articleID"
					+" HAVING 1 = COUNT(C.commentID)"
					+" ORDER BY A.name ASC";
					
		
		
			Statement statement = con.createStatement();
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				String aname= rs.getString(1);
				String uname = rs.getString(2);
				String adate = rs.getString(3);
				int rat = rs.getInt(4);
				NameUsernameDateRatingResult x = new NameUsernameDateRatingResult(aname, uname, adate, rat);
				array.add(x);
				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new NameUsernameDateRatingResult[array.size()]);
	}

	@Override
	public UsernameDateRatingResult[] UsernameDateRatingHasHighestReputation(
			String date) {
		// TODO Auto-generated method stub

		    
			ArrayList<UsernameDateRatingResult> array = new ArrayList<UsernameDateRatingResult>();
			try{
				
				//fixle
				String query = 
						" SELECT DISTINCT U.username, A.date, A.rating"
						+" FROM user U, article A, reputation R"
						+" WHERE A.date <" + "\'" + date + "\'" +  " AND"
						+ " U.userID = A.userID AND"
						+ " U.userID = R.userID AND"
						+ " R.weeklyReputation >= ALL"
						+" ("
						+" SELECT R2.weeklyReputation"
						+" FROM article A2, user U2, reputation R2"
						+" WHERE A2.date <" + "\'" + date + "\'" + " AND"
						+" U2.userID = A2.userID AND"
						+" U2.userID = R2.userID"
						+") "
						+"ORDER BY U.username";
						

			
				Statement statement = con.createStatement();
				ResultSet rs = statement.executeQuery(query);

				
				while(rs.next()){
					String uname= rs.getString(1);
					String dat = rs.getString(2);
					int rat = rs.getInt(3);
					UsernameDateRatingResult x = new UsernameDateRatingResult(uname, dat, rat);
					array.add(x);
					//System.out.println(x.toString());
				}
				statement.close();
				
			}catch(SQLException e){
				e.printStackTrace();
			}
			return array.toArray(new UsernameDateRatingResult[array.size()]);
	}

	@Override
	public UsernameDateRatingResult[] UsernameDateRatingDeleteAndSelect(
			String date) {
		ArrayList<UsernameDateRatingResult> array = new ArrayList<UsernameDateRatingResult>();
		try{
			
			//fixle
			
			String delete = 
					"DELETE FROM article WHERE articleID IN"
					+" (SELECT temp.articleID "
					+" FROM   user U, (SELECT * FROM article) AS temp"
					+" WHERE temp.date >" +  "\'"  + date  + "\'" 
					+   " AND temp.userID = U.userID)";
			
			String query = 
					" SELECT U.username, A.date, A.rating "
					+" FROM  user U, article A" 
					+   " WHERE U.userID = A.userID"
					+ " ORDER BY U.username ASC";
					

		
			Statement statement = con.createStatement();
			statement.executeUpdate(delete);
			
			ResultSet rs = statement.executeQuery(query);

			
			while(rs.next()){
				
				String uname= rs.getString(1);
				String dat = rs.getString(2);
				int rat = rs.getInt(3);
				UsernameDateRatingResult x = new UsernameDateRatingResult(uname, dat, rat);
				array.add(x);

				//System.out.println(x.toString());
			}
			statement.close();
			
		}catch(SQLException e){
			e.printStackTrace();
		}
		return array.toArray(new UsernameDateRatingResult[array.size()]);
	}

}
