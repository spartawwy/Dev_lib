#ifndef WZE_ERROR_H_
#define WZE_ERROR_H_

#include <TLib/core/tsystem_error.h>
 
using namespace TSystem;

	class WZEErrorCategory: public TSystem::TErrorCategory
	{
	public:

		enum class ErrorCode : short
		{
			OK  =  0,

			// user related
			INVALID_LOGIN,
			USER_NOT_LOGIN,
			USER_NOT_MATCH,
			USER_NO_TRADING_RIGHT,
			LOGIN_FORBIDDEN,

			// symbol related
			BAD_SYMBOL,
			PRODUCT_SYMBOL_NOT_FOUND,
			BAD_SYMBOL_PARAMETER,

			// flow control related
			DUPLICATE_QUOTE_REQUEST_SUBMIT,
			DUPLICATE_QUOTE_RESPONSE_SUBMIT,
			DUPLICATE_QUOTE_BOOK_SUBMIT,
			DUPLICATE_QUOTE_REQUEST_CANCEL,
			DUPLICATE_QUOTE_RESPONSE_CANCEL,
            DUPLICATE_QUOTE_TRIAL_SUBMIT,
			OVER_QUOTE_MESSAGE_PER_SECOND,
			OVER_QUERY_REQUEST_PER_SECOND,
            OVER_QUOTE_TRIAL_PER_SECOND,

			// query related
			BAD_QUERY_TYPE,

			// quote related
			NOT_TRADING_TIME,
			BAD_PRICE,
			BAD_QUOTE_FIELD,
			BUYSELL_FLAG_NOT_MATCH,
			OVER_QUOTE_REQUEST_QUANTITY,
			QUOTE_STATUS_NOT_MATCH,
			QUOTE_BOOK_FIELD_NOT_MATCH,
			QUOTE_REQUEST_NOT_FOUND,
			QUOTE_RESPONSE_NOT_FOUND,

			// account related
			USER_ACCOUNT_NOT_FOUND,
			CLEARING_ACCOUNT_NOT_FOUND,
			TRADING_ACCOUNT_NOT_FOUND,
			PAY_ACCOUNT_NOT_FOUND,
			MARGIN_ACCOUNT_NOT_FOUND,
			PAY_MARGIN_POSITION_NOT_FOUND,
			QUOTING_POSITION_NOT_FOUND,
			MARGIN_POSITION_NOT_FOUND,
			COST_POSITION_NOT_FOUND,
			PAY_MARGIN_POSITION_NOT_SUFFICIENT,
			QUOTING_POSITION_NOT_SUFFICIENT,
			MARGIN_POSITION_NOT_SUFFICIENT,
			COST_POSITION_NOT_SUFFICIENT,
			OVER_CLOSE_POSITION,
			SHORT_SELL_NOT_SUPPORTED,

			// transfer related
			NOT_TRANSFER_TIME,
			BAD_TRANSFER_FIELD,
			TRANSFER_NOT_FOUND,
			TRANSFER_INFO_NOT_MATCH,
			OVER_TRANSFER_POSITION,
			DUPLICATE_TRANSFER_SUBMIT,
			DUPLICATE_TRANSFER_CONFIRM,
			OVER_TRANSFER_TIMES_PER_SECOND,
			OVER_TRANSFER_TIMES_PER_DAY,
			OVER_TRANSFER_AMOUNT_PER_SECOND,
			OVER_TRANSFER_AMOUNT_PER_DAY,
		};

		// singleton accessor
		static WZEErrorCategory& GetCategory(){}
		
		virtual const std::string& name() const override {}

		virtual bool CheckOK(short ec) const override{}

		virtual const std::string& Message(short ec) const override{}

	private:

		WZEErrorCategory(){}
	};

	// make a TError based on WZEErrorCategory
	TSystem::TError     MakeTError( WZEErrorCategory::ErrorCode ec )
	{
		// todo:
		return TError();
	}

	// make a TException based on WZEErrorCategory
	TSystem::TException MakeTException( WZEErrorCategory::ErrorCode ec )
	{
		// todo:
		TError eror = MakeTError(ec);
		return TException(eror);
	}
 

#endif // WZE_ERROR_H_