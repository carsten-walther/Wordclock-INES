/**
 * AjaxServiceController
 */
export class AjaxServiceController {

  /**
   * baseUrl
   */
    public baseUrl: string = location.origin + '/';
  //public baseUrl: string = 'http://wordclock.local/';

  /**
   * getSettings
   */
  getSettings = async (): Promise<any> => {
    return fetch(this.baseUrl + 'settings.json')
      .then(response => {
        if (!response.ok) {
          throw new Error(response.statusText);
        }
        return response.json();
      })
      .then(data => {
        return data;
      });
  };

  /**
   * setSettings
   *
   * @param field
   * @param value
   */
  setSettings = async (field: string, value: any): Promise<any> => {
    let data = new FormData();

    if ((typeof field !== "undefined" || field !== null) && (typeof value !== "undefined" || value !== null)) {
      data.append(field, value);
    }

    fetch(this.baseUrl + 'update.json',{
      method: 'POST',
      body: data
    })
      .then(response => {
        if (!response.ok) {
          throw new Error(response.statusText);
        }
        return response.json();
      })
      .then(data => {
        return data;
      });
  };
}

export const AjaxService = new AjaxServiceController();
