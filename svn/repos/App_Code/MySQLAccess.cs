using System;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.HtmlControls;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Xml.Linq;

using MySql.Data.MySqlClient;

public class MySQLAccess
{
    private String mainDatabaseName = "";
    private String archiveDatabaseName = "";
    private String selectString = "";
    private String windFarmId = "";
    private String dataTableName = "";
    private String dataFieldName = "";
    private String timeFieldName = "";
    private String lastForecastTableName = "";

    public MySQLAccess()
    {
    }

    protected MySqlConnection connectToDatabase(string databaseName)
    {
        MySqlConnection mySqlConnection = null;
        string connectionString = "Database=" + databaseName + ";Data Source=localhost;User Id=root;Password=DSoFtr00t!";

        try
        {
            mySqlConnection = new MySqlConnection(connectionString);
        }
        catch (Exception ex)
        {
            throw ex;
        }

        return mySqlConnection;
    }

    public void setWindFarmId(String windFarmId)
    {
        this.windFarmId = windFarmId;
    }

    public void setDataTableName(String dataTableName)
    {
        this.dataTableName = dataTableName;
        if (dataTableName.Contains("Scada"))
        {
            setTimeFieldName("Scada_time");
        }
        else
        {
            setTimeFieldName("Forecast_time");
        }
    }

    public void setDataFieldName(String dataFieldName)
    {
        this.dataFieldName = dataFieldName;
    }

    public void setTimeFieldName(String timeFieldName)
    {
        this.timeFieldName = timeFieldName;
    }

    public void setLastForecastTableName()
    {
        string selectString = "show tables like \"iwindf1%\"";
        int count = 0;
        MySqlConnection mySqlConnection = null;

        try
        {
            mySqlConnection = connectToDatabase(archiveDatabaseName);

            MySqlCommand mySqlCommand = mySqlConnection.CreateCommand();

            mySqlCommand.CommandText = selectString;

            MySqlDataAdapter mySqlDataAdapter = new MySqlDataAdapter();

            mySqlDataAdapter.SelectCommand = mySqlCommand;

            System.Data.DataSet dataSet = new System.Data.DataSet();

            mySqlConnection.Open();

            string dataTableName = "forecastTables";
            mySqlDataAdapter.Fill(dataSet, dataTableName);

            DataTable dataTable = dataSet.Tables[dataTableName];
            count = dataTable.Rows.Count;
            if (count > 0)
            {
                DataRow dataRow = dataTable.Rows[count - 1];
                lastForecastTableName = (String)dataRow[0];
            }
            else
            {
                throw new Exception("Database does not contain forecast tables");
            }
        }
        catch (Exception ex)
        {
            throw ex;
        }
        finally
        {
            if (mySqlConnection != null)
            {
                mySqlConnection.Close();
            }
        }
    }

    public String getLastForecastTableName()
    {
        return lastForecastTableName;
    }

    public void setMainDatabaseName(String mainDatabaseName)
    {
        this.mainDatabaseName = mainDatabaseName;
    }

    public String getArchiveDatabaseName()
    {
        return archiveDatabaseName;
    }

    public void setArchiveDatabaseName(String archiveDatabaseName)
    {
        this.archiveDatabaseName = archiveDatabaseName;
    }

    public void setArchiveDatabaseName()
    {
        string selectString = "SELECT ArchiveName FROM archivelist";
        MySqlConnection mySqlConnection = null;

        try
        {
            mySqlConnection = connectToDatabase(mainDatabaseName);

            MySqlCommand mySqlCommand = mySqlConnection.CreateCommand();

            mySqlCommand.CommandText = selectString;

            MySqlDataAdapter mySqlDataAdapter = new MySqlDataAdapter();

            mySqlDataAdapter.SelectCommand = mySqlCommand;

            System.Data.DataSet dataSet = new System.Data.DataSet();

            mySqlConnection.Open();

            string dataTableName = "archiveList";
            mySqlDataAdapter.Fill(dataSet, dataTableName);

            DataTable dataTable = dataSet.Tables[dataTableName];
            DataRow dataRow = dataTable.Rows[dataTable.Rows.Count - 1];
            archiveDatabaseName = (String)dataRow[0];

            archiveDatabaseName = archiveDatabaseName.Remove(archiveDatabaseName.LastIndexOf("."));
        }
        catch (Exception ex)
        {
            throw ex;
        }
        finally
        {
            if (mySqlConnection != null)
            {
                mySqlConnection.Close();
            }
        }
    }

    private void createDataSelectString(String startTime, String endTime)
    {
        selectString = "SELECT " + dataFieldName + ", " + timeFieldName + " FROM " + dataTableName + " WHERE wf_id = " + windFarmId + " AND (" + timeFieldName + " BETWEEN \"" +
            startTime + "\" AND \"" + endTime + "\") ORDER BY " + timeFieldName + ";";
    }

    public PlotData getData(String startTime, String endTime)
    {
        int count = 0;
        PlotData plotData = null;
        MySqlConnection mySqlConnection = connectToDatabase(archiveDatabaseName);

        createDataSelectString(startTime, endTime);

        MySqlCommand mySqlCommand = mySqlConnection.CreateCommand();

        mySqlCommand.CommandText = selectString;

        MySqlDataAdapter mySqlDataAdapter = new MySqlDataAdapter();

        mySqlDataAdapter.SelectCommand = mySqlCommand;

        System.Data.DataSet dataSet = new System.Data.DataSet();

        mySqlConnection.Open();

        string dataTableName = "data";
        try
        {
            mySqlDataAdapter.Fill(dataSet, dataTableName);

            DataTable dataTable = dataSet.Tables[dataTableName];

            int i = 0;

            if ((count = dataTable.Rows.Count) > 0)
            {
                plotData = new PlotData(count);

                foreach (DataRow dataRow in dataTable.Rows)
                {
                    plotData.add(i++, Convert.ToDouble(dataRow[dataFieldName].ToString()), dataRow[timeFieldName].ToString());
                }
            }
        }
        catch (Exception e)
        {
            Message.setErrorMessage("Error accessing archive database " + e.Message);
        }

        mySqlConnection.Close();

        return plotData;
    }
}
