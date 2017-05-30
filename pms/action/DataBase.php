<?php
/**
* @package Database Class
* @author injection (mail:injection.mail@gmail.com)
* @version 1.0
*/
include_once "dbconfig.php";
include_once "libs/classes/db_mysqli.class.php";

@ini_set( 'display_errors', 0 );
class DataBase {
    private $dbConn;

    function __construct(){
       $this->connectDb();
   }
   function connectDb( $Conn_Obj ){
    global $host, $user, $pass, $dbname;
       $config=array(
        "autoconnect"=>1,
        "hostname"=>$host,
        "username"=>$user,
        "password"=>$pass,
        "database"=>$dbname
        );
       $this->dbConn=new db_mysqli();
       $this->dbConn->open($config);
   }
   function __deconstruct( ) {
       $this->dbConn->close( );
   }
}
/**
* @package Making Sqls Class exetends Database Class
* @author injection (mail:injection.mail@gmail.com)
* @version 1.0
*/
class MakeSql extends DataBase
{
    private $mSql;

    function MakeSql( $type, $arr_colum_list, $arr_sql_choice )
    {
        $this -> MakeSqlType($type, $arr_colum_list, $arr_sql_choice );
    }
    //switch make list
    function MakeSqlType( $type, $arr_colum_list, $arr_sql_choice )
    {
        switch ( $type )
        {
            case 'insert':
                return $this -> makeInsert( $arr_colum_list, $arr_sql_choice );
            case 'select':
                return $this -> makeSelect( $arr_colum_list, $arr_sql_choice );
            case 'update':
                return $this -> makeUpdate( $arr_colum_list, $arr_sql_choice );
            case 'delete':
                return $this -> makeDelete( $arr_colum_list, $arr_sql_choice );
        }
    }
    //make insert
    function makeInsert( $arr_colum_list, $arr_sql_choice )
    {
        $colum_key = array_keys( $arr_colum_list );
        $colum_value = array_values( $arr_colum_list );
        $this -> mSql = "INSERT INTO ".$arr_sql_choice["tbl_name"]."( ".join( ',' , $colum_key )." ) VALUES( '".join( "','" ,
        $colum_value )."')";
        return $this -> mSql;
    }
    //making select
    function makeSelect( $arr_colum_list = '*' , $arr_sql_choice )
    {
        $colum_value = array_keys( $arr_colum_list );
        foreach( $arr_sql_choice as $sql_key => $sql_value )
        {
            if ( strcmp( $sql_key, 'tbl_name' ) == 0 )
            {
                //if ( strcmp($arr_colum_list, '*' ) !== 0 ) {
                if(strcmp($colum_value, '*') !== 0)
                    $this -> mSql = "SELECT ".join( ',' , $colum_value )." FROM ".$sql_value;
                }
                else {
                    $this -> mSql = "SELECT * FROM ".$sql_value;
                }
            }
            else if ( strcmp( $sql_value, '' ) !== 0 )
            {
                if (strcmp( $sql_key, 'WHERE' ) === 0 && strcmp( $sql_value, 'colum' ) === 0 )
                {
                    foreach($arr_colum_list as $colum_key => $colum_value ) {
                        $this->mSql .= "$colum_key = '$colum_value' AND ";
                    }
                    $this -> mSql = rtrim( $this -> mSql, " AND " );
                }
                else {
                    $this -> mSql .= " $sql_key ".$sql_value;
                };
            }
        }
        return $this -> mSql;
    }
    //making update
    function makeUpdate( $arr_colum_list, $arr_sql_choice )
    {
        $this -> mSql = "UPDATE ".$arr_sql_choice['tbl_name']." SET ";
        foreach( $arr_colum_list as $colum_key => $colum_value ) $this -> mSql .= "$colum_key = '$colum_value',";
        $this -> mSql = rtrim( $this -> mSql , ',');
        foreach( $arr_sql_choice as $sql_key => $sql_value )
        {
            if ( strcmp( $sql_value, '' ) !== 0 && strcmp( $sql_key, 'tbl_name' ) !== 0 && strcmp( $sql_key,
            'ORDER BY' ) !== 0 ) $this -> mSql .= " $sql_key ".$sql_value;
        }
        return $this -> mSql;
    }
    //making delete
    function makeDelete( $arr_colum_list, $arr_sql_choice )
    {
        $this -> mSql = "DELETE FROM ".$arr_sql_choice['tbl_name'];
        foreach( $arr_sql_choice as $sql_key => $sql_value )
        {
            if ( strcmp( $sql_key, 'tbl_name' ) !== 0 && strcmp( $sql_value, '' ) !== 0 ) {
                $this -> mSql .= " $sql_key ".$sql_value;
            }
        }
        return $this -> mSql;
    }
}
