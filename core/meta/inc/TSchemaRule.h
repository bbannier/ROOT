// @(#)root/core:$Id$
// author: Lukasz Janyst <ljanyst@cern.ch>

#ifndef ROOT_TSchemaRule
#define ROOT_TSchemaRule

class TBuffer;
class TVirtualObject;
class TObjArray;

#include "TObject.h"
#include "Rtypes.h"
#include "TString.h"

#include <vector>
#include <utility>

namespace ROOT {

   class TSchemaRule: public TObject
   {
      public:

         typedef enum
         {
            kReadRule    = 0,
            kReadRawRule = 1
         }  RuleType_t;

         typedef void (*ReadFuncPtr_t)( char*, TVirtualObject* );
         typedef void (*ReadRawFuncPtr_t)( char*, TBuffer&);

         TSchemaRule();
         virtual ~TSchemaRule();

         TSchemaRule( const TSchemaRule& rhs );
         TSchemaRule& operator = ( const TSchemaRule& rhs );

         Bool_t           SetVersion( const TString& version );
         Bool_t           TestVersion( Int_t version ) const;
         Bool_t           SetChecksum( const TString& checksum );
         Bool_t           TestChecksum( UInt_t checksum ) const;
         void             SetSourceClass( const TString& classname );
         TString          GetSourceClass() const;
         void             SetTarget( const TString& target );
         const TObjArray* GetTarget() const;
         void             SetSource( const TString& source );
         const TObjArray* GetSource() const;
         void             SetEmbed( const Bool_t embed );
         Bool_t           GetEmbed() const;
         Bool_t           IsValid() const;
         void             SetCode( const TString& code );
         TString          GetCode() const;
         Bool_t           HasTarget( const TString& target ) const;
         Bool_t           HasSource( const TString& source ) const;
         void             SetReadFunctionPointer( ReadFuncPtr_t ptr );
         ReadFuncPtr_t    GetReadFunctionPointer() const;
         void             SetReadRawFunctionPointer( ReadRawFuncPtr_t ptr );
         ReadRawFuncPtr_t GetReadRawFunctionPointer() const;
         void             SetInclude( const TString& include );
         const TObjArray* GetInclude() const;
         void             SetRuleType( RuleType_t type );
         RuleType_t       GetRuleType() const;
         Bool_t           Conflicts( const TSchemaRule* rule ) const;

         ClassDef( TSchemaRule, 1 );

      private:

         Bool_t ProcessVersion( const TString& version ) const;
         Bool_t ProcessChecksum( const TString& checksum ) const;
         void   ProcessList( TObjArray* array, const TString& list ) const;

         TString                      fVersion;        //  Source version string
         mutable std::vector<std::pair<Int_t, Int_t> >* fVersionVect;    //! Source version vector (for searching purposes)
         TString                      fChecksum;       //  Source checksum string
         mutable std::vector<UInt_t>* fChecksumVect;   //! Source checksum vector (for searching purposes)
         TString                      fSourceClass;    //  Source clss
         TString                      fTarget;         //  Target data mamber string
         mutable TObjArray*           fTargetVect;     //! Target data member vector (for searching purposes)
         TString                      fSource;         //  Source data member string
         mutable TObjArray*           fSourceVect;     //! Source data member vector (for searching purposes)
         TString                      fInclude;        //  Includes string
         mutable TObjArray*           fIncludeVect;    //! Includes vector
         TString                      fCode;           //  User specified code snippet
         Bool_t                       fEmbed;          //  Value determining if the rule should be embedded
         ReadFuncPtr_t                fReadFuncPtr;    //! Conversion function pointer for read rule
         ReadRawFuncPtr_t             fReadRawFuncPtr; //! Conversion function pointer for readraw rule
         RuleType_t                   fRuleType;       //  Type of the rule
   };
} // End of namespace ROOT

#endif // ROOT_TSchemaRule
